"""Module for working with timeseries data from various sensors."""

TIMESERIES_WINDOW_SECONDS = 3600 * 4
TIMESERIES_MAX_VALUES = 200

class DataPoint(object):
    def __init__(self, timestamp=None, value=None):
        self.timestamp = timestamp
        self.value = value

    def maybe_extend_datapoint_range(self, new_datapoint):
        """Checks if the new data point can be assimilated into this one.

        If the new data point has the same value as this one, then we
        can simply extend the range of this data point instead of creating
        another one in the timeseries.
        """
        return new_datapoint.value == self.value


class TimeSeries(object):
    """A space-aware run-length encoded time series."""
    def __init__(self,
                 max_data_points=TIMESERIES_MAX_VALUES,
                 time_series_window_size=TIMESERIES_WINDOW_SECONDS):
        self.max_data_points = max_data_points
        self.time_series_window_size = time_series_window_size
        self.last_timestamp = 0
        self.ts_window = []

    def add(self, data_point):
        self.last_timestamp = data_point.timestamp
        if not self.ts_window:
            # Simple case, there's no data yet.
            self.ts_window.append(data_point)
            return
        last_data_point = self.ts_window[-1]
        if not last_data_point.maybe_extend_datapoint_range(data_point):
            # Couldn't just extend the range of the last data point, we
            # need a new data point.
            self.ts_window.append(data_point)
        self._maybe_trim_timeseries_window()
        self._maybe_trim_timeseries_size()

    def _maybe_trim_timeseries_window(self):
        while True:
            first, last = self.ts_window[0], self.ts_window[-1]
            window_size_to_trim = (
                self.last_timestamp - first.timestamp
                - self.time_series_window_size)
            if window_size_to_trim <= 0:
                return
            new_window_start = first.timestamp + window_size_to_trim
            print 'OK: Need to trim %d seconds.' % window_size_to_trim
            print 'OK: Old start = %d, new = %d.' % (
                    first.timestamp, new_window_start)
            for i in range(0, len(self.ts_window)):
                datapoint = self.ts_window[i]
                if new_window_start < datapoint.timestamp:
                    assert i > 0, "New start of window not in series?"
                    self.ts_window = self.ts_window[i - 1:]
                    self.ts_window[0].timestamp = new_window_start
                    return

    def _maybe_trim_timeseries_size(self):
        # If there are too many values, we'll just keep discarding old
        # ones till we're back to normal.
        while len(self.ts_window) > TIMESERIES_MAX_VALUES:
            self.ts_window = self.ts_window[1:]

    def to_array(self):
        ret_array = [[t.timestamp, t.value] for t in self.ts_window]
        if ret_array:
            if ret_array[-1][0] != self.last_timestamp:
                ret_array.append([self.last_timestamp, ret_array[-1][1]])
        return ret_array


if __name__ == '__main__':
    dp1 = DataPoint(timestamp=1, value=7777)
    dp2 = DataPoint(timestamp=10, value=7777)
    dp3 = DataPoint(timestamp=15, value=7777)
    dp4 = DataPoint(timestamp=21, value=8888)
    print 'Checking if run-length encoding works...'
    ts = TimeSeries()
    for d in [dp1, dp2, dp3, dp4]:
        ts.add(d)
    result = ts.to_array()
    assert result == [[1, 7777], [21, 8888]], 'Unexpected array: %s' % result

    print 'Checking if trimming of timeseries windows works...'
    ts = TimeSeries(time_series_window_size=18)
    for d in [dp1, dp2, dp3, dp4]:
        ts.add(d)
    result = ts.to_array()
    assert result == [[3, 7777], [21, 8888]], 'Unexpected array: %s' % result


    
