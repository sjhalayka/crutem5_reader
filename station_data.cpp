#include "station_data.h"


year_data::year_data(void)
{
	temperature_anomalies[0] = temperature_anomalies[1] = temperature_anomalies[2] = temperature_anomalies[3] = 
	temperature_anomalies[4] = temperature_anomalies[5] = temperature_anomalies[6] = temperature_anomalies[7] = 
	temperature_anomalies[8] = temperature_anomalies[9] = temperature_anomalies[10] = temperature_anomalies[11] = 0;
}

bool year_data::operator==(const year_data &rhs) const
{
	if(temperature_anomalies[0] == rhs.temperature_anomalies[0] && 
		temperature_anomalies[1] == rhs.temperature_anomalies[1] && 
		temperature_anomalies[2] == rhs.temperature_anomalies[2] && 
		temperature_anomalies[3] == rhs.temperature_anomalies[3] && 
		temperature_anomalies[4] == rhs.temperature_anomalies[4] && 
		temperature_anomalies[5] == rhs.temperature_anomalies[5] && 
		temperature_anomalies[6] == rhs.temperature_anomalies[6] && 
		temperature_anomalies[7] == rhs.temperature_anomalies[7] && 
		temperature_anomalies[8] == rhs.temperature_anomalies[8] && 
		temperature_anomalies[9] == rhs.temperature_anomalies[9] && 
		temperature_anomalies[10] == rhs.temperature_anomalies[10] && 
		temperature_anomalies[11] == rhs.temperature_anomalies[11])
	{
		return true;
	}

	return false;
}

ostream& operator<<(ostream &out, const year_data &y)
{
	out << y.temperature_anomalies[0] << ' ' << y.temperature_anomalies[1] << ' ' << y.temperature_anomalies[2] << ' ' 
		<< y.temperature_anomalies[3] << ' ' << y.temperature_anomalies[4] << ' ' << y.temperature_anomalies[5] << ' ' 
		<< y.temperature_anomalies[6] << ' ' << y.temperature_anomalies[7] << ' ' << y.temperature_anomalies[8] << ' ' 
		<< y.temperature_anomalies[9] << ' ' << y.temperature_anomalies[10] << ' ' << y.temperature_anomalies[11];

	return out;
}

station_data::station_data(void)
{
	name = country = "";
	latitude = longitude = elevation = 0;
}

bool station_data::operator==(const station_data &rhs) const
{
	if(years == rhs.years &&
		name == rhs.name &&
		country == rhs.country &&
		latitude == rhs.latitude &&
		longitude == rhs.longitude &&
		elevation == rhs.elevation)
		return true;

	return false;
}

ostream& operator<<(ostream &out, const station_data &s)
{
	out << s.name << ", " << s.country << ' ' << s.latitude << ' ' << s.longitude << ' ' << s.elevation << endl;

	for(map<short unsigned int, year_data>::const_iterator cy = s.years.begin(); cy != s.years.end(); cy++)
		out << cy->first << ' ' << cy->second << endl;

	return out;
}

float regline_slope(const vector<complex<float>> &xy)
{
	float x_mean = 0;
	float y_mean = 0;

	for(size_t i = 0; i < xy.size(); i++)
	{
		x_mean += xy[i].real();
		y_mean += xy[i].imag();
	}

	x_mean /= xy.size();
	y_mean /= xy.size();

	float covariance = 0;
	float variance = 0;

	for(size_t i = 0; i < xy.size(); i++)
	{
		float z = xy[i].real() - x_mean;
		covariance += z*(xy[i].imag() - y_mean);
		variance += z*z;
	}

	// These two divisions can be commented out
	// because they do not affect the ratio of
	// covariance and variance
	covariance /= xy.size();
	variance /= xy.size();

	return covariance / variance;
}

float standard_deviation(const vector<float> &src)
{
	float mean = 0;
	float size = static_cast<float>(src.size());

	for(size_t i = 0; i < src.size(); i++)
		mean += src[i];

	mean /= size;

	float sq_diff = 0;

	for(size_t i = 0; i < src.size(); i++)
	{
		float diff = src[i] - mean;
		sq_diff += diff*diff;
	}

	sq_diff /= size;

	return sqrt(sq_diff);
}

bool get_data(map<long unsigned int, station_data>& sd)
{
	sd.clear();

	ifstream infile("stat4.postqc.CRUTEM.5.0.1.0-202109.txt");

	if (infile.fail())
	{
		cout << "Could not load file stat4.postqc.CRUTEM.5.0.1.0-202109.txt" << endl;
		return false;
	}

	string line;

	size_t station_count = 1;

	while (getline(infile, line))
	{
		if (line == "")
			continue;

		if (station_count % 1000 == 0)
			cout << "Read " << station_count << " stations." << endl;

		station_data s;

		istringstream iss(line);

		string station_id = line.substr(0, 6);
		string latitude = line.substr(6, 4);
		string longitude = line.substr(10, 5);
		string elevation = line.substr(15, 5);
		string name = line.substr(21, 19);
		string country = line.substr(41, 12);
		string first_year = line.substr(56, 4);
		string last_year = line.substr(60, 4);

		station_id = trim(station_id);
		latitude = trim(latitude);
		longitude = trim(longitude);
		elevation = trim(elevation);
		name = trim(name);
		country = trim(country);
		first_year = trim(first_year);
		last_year = trim(last_year);

		short unsigned int fy;
		short unsigned int ly;

		istringstream iss_year(first_year + " " + last_year);
		iss_year >> fy;
		iss_year >> ly;

		size_t num_years = 1 + static_cast<size_t>(ly) - static_cast<size_t>(fy);

		long unsigned int station_id_int;

		iss.clear();
		iss.str(station_id);
		iss >> station_id_int;

		iss.clear();
		iss.str(latitude);
		iss >> s.latitude;

		iss.clear();
		iss.str(longitude);
		iss >> s.longitude;

		iss.clear();
		iss.str(elevation);
		iss >> s.elevation;

		s.name = name;
		s.country = country;

		for (size_t i = 0; i < num_years; i++)
		{
			string yd;
			getline(infile, yd);

			istringstream iss_year(yd);

			year_data y;
			short unsigned int year;

			iss_year >> year;

			for (size_t j = 0; j < 12; j++)
				iss_year >> y.temperature_anomalies[j];

			s.years[year] = y;
		}

		sd[station_id_int] = s;
		station_count++;
	}

	cout << "Read " << sd.size() << " stations altogether." << endl;

	return true;
}

void get_local_trends(const station_data &s, const short unsigned int& first_year, const short unsigned int& last_year, vector<float>& output_trends, const size_t min_samples_per_slope)
{
	output_trends.clear();

	// x is year, y is temperature
	// one vector per month
	vector<complex<float>> xy[12];

	for(map<short unsigned int, year_data>::const_iterator cy = s.years.begin(); cy != s.years.end(); cy++)
	{
		if(cy->first < first_year || cy->first > last_year)
			continue;

		for(size_t k = 0; k < 12; k++)
			if((cy->second).temperature_anomalies[k] != -999.0f) // -999 indicates missing record
				xy[k].push_back(complex<float>(static_cast<float>(cy->first), (cy->second).temperature_anomalies[k]));
	}

	for(size_t j = 0; j < 12; j++)
		if(min_samples_per_slope <= xy[j].size())
			output_trends.push_back(regline_slope(xy[j]));
}


void get_min_and_max_years(const map<long unsigned int, station_data>& sd, short unsigned int& min_year, short unsigned int& max_year)
{
	unsigned short int min = 10000;
	unsigned short int max = 0;

	for (map<long unsigned int, station_data>::const_iterator cs = sd.begin(); cs != sd.end(); cs++)
	{
		for (map<short unsigned int, year_data>::const_iterator cy = cs->second.years.begin(); cy != cs->second.years.end(); cy++)
		{
			if (cy->first < min)
				min = cy->first;

			if (cy->first > max)
				max = cy->first;
		}
	}

	min_year = min;
	max_year = max;
}

void get_mean_and_std_deviation(map<long unsigned int, station_data>& sd, const short unsigned int min_year, const short unsigned int max_year, float& mean, float& std_dev, const size_t min_years_per_slope)
{
	vector<float> all_output_trends;

	for (map<long unsigned int, station_data>::const_iterator cs = sd.begin(); cs != sd.end(); cs++)
	{
		vector<float> local_output_trends;

		get_local_trends(sd[cs->first], min_year, max_year, local_output_trends, min_years_per_slope);

		// cout << cs->first << ' ' << cs->second.name << ' ' << cs->second.country << ' ' << local_output_trends.size() << " trends" << endl;

		for (size_t i = 0; i < local_output_trends.size(); i++)
			all_output_trends.push_back(local_output_trends[i]);
	}

	float slope_mean = 0;

	for (size_t i = 0; i < all_output_trends.size(); i++)
		slope_mean += all_output_trends[i];

	slope_mean /= all_output_trends.size();

	mean = slope_mean;
	std_dev = standard_deviation(all_output_trends);
}

void write_trend_histogram(const map<long unsigned int, station_data>& sd, long unsigned int num_histogram_bins, const size_t min_samples_per_slope)
{
	vector<float> slopes;

	for (map<long unsigned int, station_data>::const_iterator cs = sd.begin(); cs != sd.end(); cs++)
	{
		// x is year, y is temperature
		// one vector per month
		vector<complex<float>> xy[12];

		for (map<short unsigned int, year_data>::const_iterator cy = cs->second.years.begin(); cy != cs->second.years.end(); cy++)
			for (size_t k = 0; k < 12; k++)
				if ((cy->second).temperature_anomalies[k] != -999.0f)
					xy[k].push_back(complex<float>(static_cast<float>(cy->first), (cy->second).temperature_anomalies[k]));

		for (size_t j = 0; j < 12; j++)
			if (min_samples_per_slope <= xy[j].size())
				slopes.push_back(regline_slope(xy[j]));
	}

	sort(slopes.begin(), slopes.end());

	float slope_min = FLT_MAX;
	float slope_max = FLT_MIN;
	double slope_mean = 0;

	for (size_t i = 0; i < slopes.size(); i++)
	{
		if (slopes[i] < slope_min)
			slope_min = slopes[i];

		if (slopes[i] > slope_max)
			slope_max = slopes[i];

		slope_mean += slopes[i];
	}

	slope_mean /= static_cast<double>(slopes.size());

	float distance = fabsf(slope_max - slope_min);
	float bin_width = distance / static_cast<float>(num_histogram_bins);
	float half_bin_width = bin_width * 0.5f;

	vector<long unsigned int> bins(num_histogram_bins);
	size_t slope_index = 0;

	for (size_t i = 0; i < num_histogram_bins; i++)
	{
		float curr_end = slope_min + (i + 1) * bin_width;

		while (slope_index < slopes.size() && slopes[slope_index] <= curr_end)
		{
			bins[i]++;
			slope_index++;
		}
	}


	float curr_mid = slope_min + half_bin_width;

//	cout << "Outputting gnuplot histogram data." << endl;
	ofstream plotdata("histogram.txt");

	for (size_t i = 0; i < num_histogram_bins; i++)
	{
		plotdata << curr_mid << ' ' << bins[i] << endl;
		curr_mid += bin_width;
	}

	ofstream plotcmd("histogram.plt");

	plotcmd << "set terminal postscript eps monochrome enhanced" << endl;
	plotcmd << "set key off" << endl;
	plotcmd << "set output \"histogram.eps\"" << endl;
	plotcmd << "set grid" << endl;
	plotcmd << "set xlabel \"Slope\"" << endl;
	plotcmd << "set ylabel \"Count\"" << endl;
	plotcmd << "set title \"Num slopes = " << slopes.size() << ", slope min = " << slope_min << ", slope max = " << slope_max << ", slope mean = " << slope_mean << ", num bins = " << num_histogram_bins << "\"" << endl;
	//	plotcmd << "set xrange [" << slope_min << ':' << slope_max << ']' << endl;
	plotcmd << "set xrange [" << -0.2 << ':' << 0.2 << ']' << endl;
	plotcmd << "plot \"histogram.txt\" with boxes" << endl;
	plotcmd << "set terminal wxt" << endl;
	plotcmd << "set output" << endl;
}

void print_all(const map<long unsigned int, station_data>& sd)
{
	// Print Station ID and station data (including years of records)
	for (map<long unsigned int, station_data>::const_iterator cs = sd.begin(); cs != sd.end(); cs++)
		cout << cs->first << ' ' << cs->second << endl;
}
