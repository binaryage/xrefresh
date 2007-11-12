#pragma once

class CServices : public CResourceInit<SR_SERVICES> {
public:
	CServices();
	~CServices();

	bool                                           OpenSettingsDialog();

private:
};
