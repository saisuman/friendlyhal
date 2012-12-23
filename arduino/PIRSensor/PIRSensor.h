class PIRSensor {
 public:
  PIRSensor();
  ~PIRSensor();
  void init(int pin);
  bool motionDetected();

 private:
  int pin_;
};
