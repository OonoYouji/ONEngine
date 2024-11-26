#pragma once


struct Flag {
	void Update() {
		prev = current;
	}

	const bool Press()   const { return current; }
	const bool Trigger() const { return current && !prev; }
	const bool Release() const { return !current && prev; }

	bool current;
	bool prev;
};