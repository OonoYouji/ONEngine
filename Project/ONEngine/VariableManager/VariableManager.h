#pragma once


class VariableManager final {
	VariableManager() {}
	~VariableManager() {}
public:

	static VariableManager* GetInstance() {
		static VariableManager instance;
		return &instance;
	}

	void Initialize();
	void Finalize();

private:
	VariableManager(const VariableManager&) = delete;
	VariableManager(VariableManager&&) = delete;
	VariableManager& operator= (const VariableManager&) = delete;
	VariableManager& operator= (VariableManager&&) = delete;
};