typedef void (*func)(void);

class PeriodicTask{
	public:
        PeriodicTask(unsigned long period, func func);
        void loop();
	
	private:
        unsigned long _lastMillis, _period;
        func _func;
};