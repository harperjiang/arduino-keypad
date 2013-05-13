/*
 * @version 1.0
 * @date May/05/2013
 * @author Harper Jiang 
 * @contact harperjiang@gmail.com
 * 
 * This library can be used to work together with a Matrix keypad 
 * for Arduino (generally of size 4x3, 3x3 or 4x4). 
 *
 * See method comments below for more usage information.
 * 
 * This library use INPUT_PULLUP thus requires your board to support
 * it.
 * 
 */


#ifndef _KEYPAD_H
#define _KEYPAD_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define UP HIGH
#define DOWN LOW

#define NONE '\0'

#define MAX_SIZE 20

#define BUFFER_SIZE 100

typedef enum {
	PRESSED, RELEASED, FREE, HOLD
} KeyState;

/**
 * Internal class that describes the information of each key
 */
class Key {
private:
	char keyval;
	KeyState state;
	int lastval;
	int holdTimer;
	bool changed;
public:
	Key();
	Key(char val);
	char getKeyval();
	void setKeyval(char kv);
	KeyState getState();
	bool isChanged();
	void setChanged(bool);
	bool setValue(int newval);
	void setHoldTimer(int);
};

class Keypad;

class KeypadListener {
public:
	virtual void keyStateChanged(Keypad* source, int keyindex, KeyState olds, KeyState news) = 0;
};

/**
 * Main class. 
 * See API comments and examples for more details
 */
class Keypad {
private:
	// basic information
	char* keymap;
	int* rowpins;
	int* colpins;
	int rowcount;
	int colcount;
	
	int size;

	Key keyList[MAX_SIZE];

	// The scanning period
	int period;
	int holdTime;
	int lastCheck;

	// Flag used to avoid unnecessary scan
	bool waitForUpdate;
	// Listener
	KeypadListener* listener;
protected:
	bool scan();
public:
    /**
     * Constructor.
	 * The charmap is the characters mapping to the keypad in row-major order. 
	 * The rowpins will be configured to be OUTPUT,
     * and the colpins will be used as INPUT_PULLUP.
     */
	Keypad(char* keymap, int* rowpins, int* colpins, int rowcount, int colcount);
	virtual ~Keypad();

	/*
     * Set the scanning period. If two scan request are closer than the period, 
	 * the second one will be ignored.
     */ 
	void setPeriod(int p);
	int getPeriod();

	// Set how long a pressed key will be recognized as HOLD status
	void setHoldTime(int h);
	int getHoldTime();

	// Init the pins' mode
	void init();

	/*
     * Here are the methods used to access the keypad
     */

	// Get the first key pressed
	char getPressed();
	
    // Test whether a key is pressed
	bool isPressed(char key);
	
    // Check state of a given key
	KeyState getState(int);
	
    // Block until a key is pressed
	char get();

	/**
     * Methods for event handling
     */
	void setListener(KeypadListener*);
};


#endif
