#ifndef IPAUSABLE_INL_
#define IPAUSABLE_INL_

/**
 * @brief Tag interface which states if 
 * System should be paused or not; default: true
 */
class IPausable
{
public:
    IPausable(bool pausable = true) : pausable(pausable) {}
    virtual ~IPausable() = default;

    /**
     * @brief returns const flag is pausable
     */
    inline const bool isPausable() { return pausable; }
protected:
private:
    bool pausable;
};

#endif /* !IPAUSABLE_INL_ */
