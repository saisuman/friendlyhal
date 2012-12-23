class TempHumSensor {
 public:
  TempHumSensor();
  void init(int pin);
  bool refresh();
  int getTemperaturex100();
  int getHumidityx100();
 private:
  int pin_;
  int curr_hum_;
  int curr_temp_;
};
