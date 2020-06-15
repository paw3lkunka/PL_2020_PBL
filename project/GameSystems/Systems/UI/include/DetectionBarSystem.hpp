#ifndef DetectionBarSystem_hpp_sh
#define DetectionBarSystem_hpp_sh

#include "System.hpp"
#include <string>

//TODO documentation
class DetectionBarSystem : public System
{
public:
	DetectionBarSystem() = default;
	virtual ~DetectionBarSystem() = default;

	/**
	 * @brief Initialize system by finding reference to progress bar by entity name.
	 * If failed, system wouldn't be warking at all.
	 * 
	 * @param barEntityName name of entity nantaining appropriate progress bar.
	 */
	bool init(const char* barEntityName);
	
	/**
	 * @brief Set proper initial state of detection bar.
	 * Should be called before process(FRAME);
	 */
	void prepare();
protected:

	virtual bool assertEntity(Entity* entity);
	virtual void frameUpdate();

private:
	std::string barEntityName;
	
	Enemy* enemyPtr;
	ProgressBar* progressBarPtr;
};

#endif
