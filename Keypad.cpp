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



#include "Keypad.h"

Key::Key() {
	state = FREE;
	lastval = UP;
}

Key::Key(char val) {
	state = FREE;
	lastval = UP;	
	keyval = val;
}

char Key::getKeyval() {
	return keyval;
}

void Key::setKeyval(char kv) {
	this->keyval = kv;
}

KeyState Key::getState() {
	return state;
}

bool Key::isChanged() {
	return changed;
}

void Key::setChanged(bool c) {
	changed = c;
}

bool Key::setValue(int newval) {
	int oldval = lastval;
	lastval = newval;
	changed = false;
	switch(state) {
		case RELEASED:
			if(oldval == newval) {
				state = FREE;
			} else {
				state = PRESSED;
			}
			changed = true;
			break;
		case PRESSED:
			if(oldval == newval) {
				// Deal with hold
				if(millis() > holdTimer) {
					state = HOLD;
					changed = true;
				} else {
					changed = false;
				}
			} else {
				state = RELEASED;
				changed = true;
			}
			break;
		case FREE:
			if(oldval == newval) {
				changed = false;
			} else {
				state = PRESSED;
				changed = true;
			}
			break;
		case HOLD:
			if(oldval == newval) {
				changed = false;
			} else {
				state = RELEASED;
				changed = true;
			}
			break;
		default:
			break;
	}
	return changed;
}

void Key::setHoldTimer(int ht) {
	this->holdTimer = ht;
}

Keypad::Keypad(char* keymap, int* rowpins, int* colpins, int rowcount, int colcount) {
	this->keymap = keymap;
	this->rowpins = rowpins;
	this->colpins = colpins;
	this->rowcount = rowcount;
	this->colcount = colcount;

	this->size = rowcount * colcount;

	this->period = 50;
	this->holdTime = 2000;
	this->lastCheck = 0;
	
	int size = rowcount * colcount;
	for(int i = 0 ; i < size; i++) {
		keyList[i].setKeyval(keymap[i]);
	}
	
	listener = NULL;

	init();
}

Keypad::~Keypad() {
}

void Keypad::init() {
	for(int i = 0; i < rowcount; i++) {
		pinMode(rowpins[i], OUTPUT);
	}
	for(int i = 0; i < colcount; i++) {
		pinMode(colpins[i], INPUT_PULLUP);
	}
}

bool Keypad::scan() {
	if(millis() - lastCheck < period) {
		if(!waitForUpdate) {
			for(int i = 0 ; i < size; i++) {
				keyList[i].setChanged(false);
			}
			waitForUpdate = true;
		}
		return false;
	}
	waitForUpdate = false;
	bool change = false;
	// scan rows and columns for pressed keys
	for(int i = 0 ; i < rowcount; i++) {
		digitalWrite(rowpins[i],HIGH);
	}
	for(int i = 0; i < rowcount; i++) {
		if(i > 0)
			digitalWrite(rowpins[i-1], HIGH);
		digitalWrite(rowpins[i], LOW);	
		for(int j = 0; j < colcount; j++) {
			Key* key = &keyList[i*rowcount+j];
			int value = digitalRead(colpins[j]);
			KeyState oldst = key->getState();
			bool keychange = key->setValue(value);
			KeyState newst = key->getState();
			change = change || keychange;
			if(value = DOWN && change) {
				// Press down, set hold timer
				key->setHoldTimer(millis()+holdTime);
			}
			// Call Listener
			if(NULL != listener && oldst != newst) {
				listener->keyStateChanged(this, i*rowcount+j, oldst, newst);
			}
		}
	}
	lastCheck = millis();
	return change;
}

char Keypad::getPressed() {
	scan();
	for(int i = 0 ; i < size; i++) {
		if(keyList[i].getState() == PRESSED && keyList[i].isChanged())
			return keyList[i].getKeyval();	
	}
	return NONE;
}

bool Keypad::isPressed(char key) {
	scan();
	for(int i = 0 ; i < size; i++) {
		if(keyList[i].getKeyval() == key)
			return PRESSED == keyList[i].getState() || HOLD == keyList[i].getState();
	} 
}

char Keypad::get() {
	char ret;
	while(NONE == (ret = getPressed())) {
		delay(period);
	}
	return ret;
}

KeyState Keypad::getState(int indx) {
	return keyList[indx].getState();
}

void Keypad::setListener(KeypadListener* l) {
	this->listener = l;
} 
