class PWG_API Noise
{
private:
	Noise();
	static Noise* instance;

public:
	float stdNoise = 0.00003;
	float xOffset = 0;
	float yOffset = 0;

	float noise(float x, float y);
	void initialize(float inX, float inY);
	static Noise* getInstance() {
		if (instance == nullptr)
			instance = new Noise();
		return instance;
	}
};
