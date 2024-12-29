#pragma once

struct Flag final {
	void Update() {
		prevFlag = currentFlag;
	}

	bool Stay() const { return currentFlag; }
	bool Enter() const { return currentFlag && !prevFlag; }
	bool Exit() const { return !currentFlag && prevFlag; }

	void Set(bool _flag) {
		currentFlag = _flag;
	}

public:

	bool currentFlag = false;
	bool prevFlag    = false;

};