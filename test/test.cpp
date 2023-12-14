#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <set>
#include <filesystem>
#include <boost/regex.hpp>
// #include <Rcpp.h>

//#include <boost/program_options.hpp>
//namespace po = boost::program_options;

using namespace std;


map<string, string> country_codes = {
  {"Afghanistan","AF"},
  {"Åland Islands","AX"},
  {"Albania","AL"},
  {"Algeria","DZ"},
  {"American Samoa","AS"},
  {"Andorra","AD"},
  {"Angola","AO"},
  {"Anguilla","AI"},
  {"Antarctica","AQ"},
  {"Antigua and Barbuda","AG"},
  {"Argentina","AR"},
  {"Armenia","AM"},
  {"Aruba","AW"},
  {"Australia","AU"},
  {"Austria","AT"},
  {"Azerbaijan","AZ"},
  {"Bahamas","BS"},
  {"The Bahamas","BS"},
  {"Bahrain","BH"},
  {"Bangladesh","BD"},
  {"Barbados","BB"},
  {"Belarus","BY"},
  {"Belgium","BE"},
  {"Belize","BZ"},
  {"Benin","BJ"},
  {"Bermuda","BM"},
  {"Bhutan","BT"},
  {"Bolivia, Plurinational State of","BO"},
  {"Bolivia","BO"},
  {"Bonaire, Sint Eustatius and Saba","BQ"},
//   {"Bonaire","BQ"},
  {"Bosnia and Herzegovina","BA"},
  {"Botswana","BW"},
  {"Bouvet Island","BV"},
  {"Brazil","BR"},
  {"British Indian Ocean Territory","IO"},
  {"Brunei Darussalam","BN"},
  {"Brunei","BN"},
  {"Bulgaria","BG"},
  {"Burkina Faso","BF"},
  {"Burundi","BI"},
  {"Cambodia","KH"},
  {"Cameroon","CM"},
  {"Canada","CA"},
  {"Cape Verde","CV"},
  {"Cabo Verde","CV"},
  {"Cayman Islands","KY"},
  {"Central African Republic","CF"},
  {"Chad","TD"},
  {"Chile","CL"},
  {"China","CN"},
  {"Christmas Island","CX"},
  {"Cocos (Keeling) Islands","CC"},
  {"Colombia","CO"},
  {"Comoros","KM"},
  {"Congo","CG"},
  {"Congo, the Democratic Republic of the","CD"},
  {"Democratic Republic of the Congo","CD"},
  {"Cook Islands","CK"},
  {"Costa Rica","CR"},
  {"Côte d'Ivoire","CI"},
  {"Cote d'Ivoire","CI"},
  {"Croatia","HR"},
  {"Cuba","CU"},
  {"Curaçao","CW"},
  {"Curacao","CW"},
  {"Cyprus","CY"},
  {"Czech Republic","CZ"},
//   {"Crimea","UA"},
  {"Denmark","DK"},
  {"Djibouti","DJ"},
  {"Dominica","DM"},
  {"Dominican Republic","DO"},
  {"Ecuador","EC"},
  {"Egypt","EG"},
  {"El Salvador","SV"},
  {"Equatorial Guinea","GQ"},
  {"Eritrea","ER"},
  {"Estonia","EE"},
  {"Ethiopia","ET"},
  {"Eswatini","SZ"},
  {"Falkland Islands (Malvinas)","FK"},
  {"Faroe Islands","FO"},
  {"Fiji","FJ"},
  {"Finland","FI"},
  {"France","FR"},
  {"French Guiana","GF"},
  {"French Polynesia","PF"},
  {"French Southern Territories","TF"},
  {"Gabon","GA"},
  {"Gambia","GM"},
  {"Georgia","GE"},
  {"Germany","DE"},
  {"Ghana","GH"},
  {"Gibraltar","GI"},
  {"Greece","GR"},
  {"Greenland","GL"},
  {"Grenada","GD"},
  {"Guadeloupe","GP"},
  {"Guam","GU"},
  {"Guatemala","GT"},
  {"Guernsey","GG"},
  {"Guinea","GN"},
  {"Guinea-Bissau","GW"},
  {"Guyana","GY"},
  {"Haiti","HT"},
  {"Heard Island and McDonald Islands","HM"},
  {"Holy See (Vatican City State)","VA"},
  {"Honduras","HN"},
  {"Hong Kong","HK"},
  {"Hungary","HU"},
  {"Iceland","IS"},
  {"India","IN"},
  {"Indonesia","ID"},
  {"Iran, Islamic Republic of","IR"},
  {"Iran","IR"},
  {"Iraq","IQ"},
  {"Ireland","IE"},
  {"Isle of Man","IM"},
  {"Israel","IL"},
  {"Italy","IT"},
  {"Jamaica","JM"},
  {"Japan","JP"},
  {"Jersey","JE"},
  {"Jordan","JO"},
  {"Kazakhstan","KZ"},
  {"Kenya","KE"},
  {"Kiribati","KI"},
  {"Korea, Democratic People's Republic of","KP"},
  {"Korea, Republic of","KR"},
  {"South Korea","KR"},
//   {"Kosovo","XK"},
  {"Kuwait","KW"},
  {"Kyrgyzstan","KG"},
  {"Lao People's Democratic Republic","LA"},
  {"Latvia","LV"},
  {"Laos","LA"},
  {"Lebanon","LB"},
  {"Lesotho","LS"},
  {"Liberia","LR"},
  {"Libya","LY"},
  {"Liechtenstein","LI"},
  {"Lithuania","LT"},
  {"Luxembourg","LU"},
  {"Macao","MO"},
  {"Macedonia, the Former Yugoslav Republic of","MK"},
  {"North Macedonia","MK"},
  {"Madagascar","MG"},
  {"Malawi","MW"},
  {"Malaysia","MY"},
  {"Maldives","MV"},
  {"Mali","ML"},
  {"Malta","MT"},
  {"Marshall Islands","MH"},
  {"Martinique","MQ"},
  {"Mauritania","MR"},
  {"Mauritius","MU"},
  {"Mayotte","YT"},
  {"Mexico","MX"},
  {"Micronesia, Federated States of","FM"},
  {"Moldova, Republic of","MD"},
  {"Moldova","MD"},
  {"Monaco","MC"},
  {"Mongolia","MN"},
  {"Montenegro","ME"},
  {"Montserrat","MS"},
  {"Morocco","MA"},
  {"Mozambique","MZ"},
  {"Myanmar","MM"},
  {"Namibia","NA"},
  {"Nauru","NR"},
  {"Nepal","NP"},
  {"Netherlands","NL"},
  {"New Caledonia","NC"},
  {"New Zealand","NZ"},
  {"Nicaragua","NI"},
  {"Niger","NE"},
  {"Nigeria","NG"},
  {"Niue","NU"},
  {"Norfolk Island","NF"},
  {"Northern Mariana Islands","MP"},
  {"Norway","NO"},
  {"Oman","OM"},
  {"Pakistan","PK"},
  {"Palau","PW"},
  {"Palestine, State of","PS"},
  {"Panama","PA"},
  {"Papua New Guinea","PG"},
  {"Paraguay","PY"},
  {"Peru","PE"},
  {"Philippines","PH"},
  {"Pitcairn","PN"},
  {"Poland","PL"},
  {"Portugal","PT"},
  {"Puerto Rico","PR"},
  {"Qatar","QA"},
  {"Réunion","RE"},
  {"Romania","RO"},
  {"Russian Federation","RU"},
  {"Russia","RU"},
  {"Rwanda","RW"},
  {"Saint Barthélemy","BL"},
  {"Saint Barthelemy","BL"},
  {"Saint Helena, Ascension and Tristan da Cunha","SH"},
  {"Saint Kitts and Nevis","KN"},
  {"Saint Lucia","LC"},
  {"Saint Martin (French part)","MF"},
  {"Saint Pierre and Miquelon","PM"},
  {"Saint Vincent and the Grenadines","VC"},
  {"Samoa","WS"},
  {"San Marino","SM"},
  {"Sao Tome and Principe","ST"},
  {"Saudi Arabia","SA"},
  {"Senegal","SN"},
  {"Serbia","RS"},
  {"Seychelles","SC"},
  {"Sierra Leone","SL"},
  {"Singapore","SG"},
  {"Sint Maarten (Dutch part)","SX"},
  {"Sint Maarten","SX"},
  {"Slovakia","SK"},
  {"Slovenia","SI"},
  {"Solomon Islands","SB"},
  {"Somalia","SO"},
  {"South Africa","ZA"},
  {"South Georgia and the South Sandwich Islands","GS"},
  {"South Sudan","SS"},
  {"Spain","ES"},
  {"Sri Lanka","LK"},
  {"Sudan","SD"},
  {"Suriname","SR"},
  {"Svalbard and Jan Mayen","SJ"},
  {"Swaziland","SZ"},
  {"Sweden","SE"},
  {"Switzerland","CH"},
  {"Syrian Arab Republic","SY"},
  {"Taiwan, Province of China","TW"},
  {"Tajikistan","TJ"},
  {"Tanzania, United Republic of","TZ"},
  {"Thailand","TH"},
  {"Timor-Leste","TL"},
  {"Togo","TG"},
  {"Tokelau","TK"},
  {"Tonga","TO"},
  {"Trinidad and Tobago","TT"},
  {"Tunisia","TN"},
  {"Turkey","TR"},
  {"Turkmenistan","TM"},
  {"Turks and Caicos Islands","TC"},
  {"Tuvalu","TV"},
  {"Uganda","UG"},
  {"Ukraine","UA"},
  {"United Arab Emirates","AE"},
  {"United Kingdom","GB"},
  {"United States","US"},
  {"United States Minor Outlying Islands","UM"},
  {"Uruguay","UY"},
  {"Uzbekistan","UZ"},
  {"Vanuatu","VU"},
  {"Venezuela, Bolivarian Republic of","VE"},
  {"Venezuela","VE"},
  {"Viet Nam","VN"},
  {"Vietnam","VN"},
  {"Virgin Islands, British","VG"},
  {"Virgin Islands, U.S.","VI"},
  {"U.S. Virgin Islands","VI"},
  {"USA","US"},
  {"Wallis and Futuna","WF"},
  {"Western Sahara","EH"},
  {"Yemen","YE"},
  {"Zambia","ZM"},
  {"Zimbabwe","ZW"},
//   {"Canary Islands", "CI"}
  };

// string get_country_Code(string country) {
//   if (country_codes.find(country) != country_codes.end())
//     return country_codes[country];
//   for (size_t i=0; i<country.size(); i++) {
//     if (country_codes.find(country.substr(0, i)) != country_codes.end()) {
//       return country_codes[country.substr(0, i)];
//     }
//   }
//   return country;
// }

vector<pair<string, string>> country_regex = {
  {"afghan", "AF"},
{"albania", "AL"},
{"algeria", "DZ"},
{"^(?=.*americ).*samoa", "AS"},
{"andorra", "AD"},
{"angola", "AO"},
{"anguill?a", "AI"},
{"antarctica", "AQ"},
{"antigua", "AG"},
{"argentin", "AR"},
{"armenia", "AM"},
{"^(?!.*bonaire).*\\baruba", "AW"},
{"australia", "AU"},
{"^(?!.*hungary).*austria|\\baustri.*\\bemp", "AT"},
{"azerbaijan", "AZ"},
{"bahamas", "BS"},
{"bahrain", "BH"},
{"bangladesh|^(?=.*east).*paki?stan", "BD"},
{"barbados", "BB"},
{"belarus|byelo", "BY"},
{"^(?!.*luxem).*belgium", "BE"},
{"belize|^(?=.*british).*honduras", "BZ"},
{"benin|dahome", "BJ"},
{"bermuda", "BM"},
{"bhutan", "BT"},
{"bolivia", "BO"},
{"herzegovina|bosnia", "BA"},
{"botswana|bechuana", "BW"},
{"bouvet", "BV"},
{"brazil", "BR"},
{"british.?indian.?ocean", "IO"},
{"^(?=.*\\bu\\.?\\s?k).*virgin|^(?=.*brit).*virgin|^(?=.*kingdom).*virgin", "VG"},
{"brunei", "BN"},
{"bulgaria", "BG"},
{"burkina|\\bfaso|upper.?volta", "BF"},
{"burundi", "BI"},
{"cambodia|kampuchea|khmer", "KH"},
{"cameroon", "CM"},
{"canada", "CA"},
{"verde", "CV"},
{"^(?=.*bonaire).*eustatius|^(?=.*carib).*netherlands|\\bbes.?islands", "BQ"},
{"cayman", "KY"},
{"\\bcentral.african.rep", "CF"},
{"\\bchad", "TD"},
{"\\bchile", "CL"},
{"^(?!.*\\bmac)(?!.*\\bhong)(?!.*\\btai)(?!.*\\brep).*china|^(?=.*peo)(?=.*rep).*china", "CN"},
{"christmas", "CX"},
{"\\bcocos|keeling", "CC"},
{"colombia", "CO"},
{"comoro", "KM"},
{"^(?!.*\\bd.m)(?!.*\\bd[\\.]?r)(?!.*kinshasa)(?!.*zaire)(?!.*belg)(?!.*l.opoldville)(?!.*free).*\\bcongo", "CG"},
{"\\bd.m.*congo|congo.*\\bd.m|congo.*\\bd[\\.]?r|\\bd[\\.]?r.*congo|belgian.?congo|congo.?free.?state|kinshasa|zaire|l.opoldville|drc|droc|rdc", "CD"},
{"\\bcook", "CK"},
{"costa.?rica", "CR"},
{"croatia", "HR"},
{"\\bcuba", "CU"},
{"^(?!.*bonaire).*\\bcura(c|ç)ao", "CW"},
{"cyprus", "CY"},
{"^(?=.*rep).*czech|czechia|bohemia", "CZ"},
{"ivoire|ivory", "CI"},
{"denmark", "DK"},
{"djibouti", "DJ"},
{"dominica(?!n)", "DM"},
{"dominican.rep", "DO"},
{"ecuador", "EC"},
{"egypt", "EG"},
{"el.?salvador", "SV"},
{"guine.*eq|eq.*guine|^(?=.*span).*guinea", "GQ"},
{"eritrea", "ER"},
{"estonia", "EE"},
{"swaziland|eswatini", "SZ"},
{"ethiopia|abyssinia", "ET"},
{"falkland|malvinas", "FK"},
{"faroe|faeroe", "FO"},
{"fiji", "FJ"},
{"finland", "FI"},
{"^(?!.*\\bdep)(?!.*martinique).*france|french.?republic|\\bgaul", "FR"},
{"french.?gu(y|i)ana", "GF"},
{"french.?polynesia|tahiti", "PF"},
{"french.?southern", "TF"},
{"gabon", "GA"},
{"gambia", "GM"},
{"^(?!.*south).*georgia", "GE"},
{"^(?!.*east).*germany|^(?=.*\\bfed.*\\brep).*german", "DE"},
{"ghana|gold.?coast", "GH"},
{"gibraltar", "GI"},
{"greece|hellenic|hellas", "GR"},
{"greenland", "GL"},
{"grenada", "GD"},
{"guadeloupe", "GP"},
{"\\bguam", "GU"},
{"guatemala", "GT"},
{"guernsey", "GG"},
{"^(?!.*eq)(?!.*span)(?!.*bissau)(?!.*portu)(?!.*new).*guinea", "GN"},
{"bissau|^(?=.*portu).*guinea", "GW"},
{"^guyana|british.?gu(y|i)ana", "GY"},
{"haiti", "HT"},
{"heard.*mcdonald", "HM"},
{"^(?!.*brit).*honduras", "HN"},
{"hong.?kong", "HK"},
{"^(?!.*austr).*hungary", "HU"},
{"iceland", "IS"},
{"india(?!.*ocea)", "IN"},
{"indonesia", "ID"},
{"\\biran|persia", "IR"},
{"\\biraq|mesopotamia", "IQ"},
{"^(?!.*north).*\\bireland", "IE"},
{"^(?=.*isle).*\\bman", "IM"},
{"israel", "IL"},
{"italy", "IT"},
{"jamaica", "JM"},
{"japan", "JP"},
{"jersey", "JE"},
{"jordan", "JO"},
{"kazak", "KZ"},
{"kenya|british.?east.?africa|east.?africa.?prot", "KE"},
{"kiribati", "KI"},
{"kuwait", "KW"},
{"kyrgyz|kirghiz", "KG"},
{"\\blaos?\\b", "LA"},
{"latvia", "LV"},
{"lebanon", "LB"},
{"lesotho|basuto", "LS"},
{"liberia", "LR"},
{"libya", "LY"},
{"liechtenstein", "LI"},
{"lithuania", "LT"},
{"^(?!.*belg).*luxem", "LU"},
{"maca(o|u)", "MO"},
{"madagascar|malagasy", "MG"},
{"malawi|nyasa", "MW"},
{"malaysia", "MY"},
{"maldive", "MV"},
{"\\bmali\\b", "ML"},
{"\\bmalta", "MT"},
{"marshall", "MH"},
{"martinique", "MQ"},
{"mauritania", "MR"},
{"mauritius", "MU"},
{"\\bmayotte", "YT"},
{"\\bmexic", "MX"},
{"fed.*micronesia|micronesia.*fed", "FM"},
{"moldov|b(a|e)ssarabia", "MD"},
{"monaco", "MC"},
{"mongolia", "MN"},
{"^(?!.*serbia).*montenegro", "ME"},
{"montserrat", "MS"},
{"morocco|\\bmaroc", "MA"},
{"mozambique", "MZ"},
{"myanmar|burma", "MM"},
{"namibia", "NA"},
{"nauru", "NR"},
{"nepal", "NP"},
{"^(?!.*\\bant)(?!.*\\bcarib).*netherlands|holland", "NL"},
{"new.?caledonia", "NC"},
{"new.?zealand", "NZ"},
{"nicaragua", "NI"},
{"\\bniger(?!ia)", "NE"},
{"nigeria", "NG"},
{"niue", "NU"},
{"norfolk", "NF"},
{"korea.*people|dprk|d.p.r.k|korea.+(d.p.r|dpr|north|dem.*rep.*)|(d.p.r|dpr|north|dem.*rep.*).+korea", "KP"},
{"macedonia|fyrom", "MK"},
{"mariana", "MP"},
{"norway", "NO"},
{"\\boman|trucial", "OM"},
{"^(?!.*east).*paki?stan", "PK"},
{"palau", "PW"},
{"palestin|\\bgaza|west.?bank", "PS"},
{"panama", "PA"},
{"papua|new.?guinea", "PG"},
{"paraguay", "PY"},
{"peru", "PE"},
{"philippines", "PH"},
{"pitcairn", "PN"},
{"poland", "PL"},
{"portugal", "PT"},
{"puerto.?rico", "PR"},
{"qatar", "QA"},
{"r(o|u|ou)mania", "RO"},
{"\\brussia|soviet.?union|u\\.?s\\.?s\\.?r|socialist.?republics", "RU"},
{"rwanda", "RW"},
{"r(e|é)union", "RE"},
{"saint.martin.*FR|^(?=.*collectivity).*martin|^(?=.*france).*martin(?!ique)|^(?=.*french).*martin(?!ique)", "MF"},
{"^(?!.*amer).*samoa", "WS"},
{"san.?marino", "SM"},
{"\\bsa\\w*.?arabia", "SA"},
{"senegal", "SN"},
{"^(?!.*monte).*serbia", "RS"},
{"seychell", "SC"},
{"sierra", "SL"},
{"singapore", "SG"},
{"^(?!.*martin)(?!.*saba).*maarten", "SX"},
{"^(?!.*cze).*slovak", "SK"},
{"slovenia", "SI"},
{"solomon", "SB"},
{"somalia", "SO"},
{"south.africa|s\\\\..?africa", "ZA"},
{"south.?georgia|sandwich", "GS"},
{"^(?!.*d.*p.*r)(?!.*democrat)(?!.*dem.*rep)(?!.*people)(?!.*north).*\\bkorea(?!.*d.*p.*r)(?!.*dem.*rep)", "KR"},
{"\\bs\\w*.?sudan", "SS"},
{"spain", "ES"},
{"sri.?lanka|ceylon", "LK"},
{"barth(e|é)lemy", "BL"},
{"helena", "SH"},
{"kitts|\\bnevis", "KN"},
{"\\blucia", "LC"},
{"miquelon", "PM"},
{"vincent", "VC"},
{"^(?!.*\\bs(?!u)).*sudan", "SD"},
{"surinam|dutch.?gu(y|i)ana", "SR"},
{"svalbard", "SJ"},
{"sweden", "SE"},
{"switz|swiss", "CH"},
{"syria", "SY"},
{"\\bs(a|ã)o.?tom(e|é)", "ST"},
{"taiwan|taipei|formosa|^(?!.*peo)(?=.*rep).*china", "TW"},
{"tajik", "TJ"},
{"tanzania", "TZ"},
{"thailand|\\bsiam", "TH"},
{"^(?=.*leste).*timor|^(?=.*east).*timor", "TL"},
{"togo", "TG"},
{"tokelau", "TK"},
{"tonga", "TO"},
{"trinidad|tobago", "TT"},
{"tunisia", "TN"},
{"turkey|t(ü|u)rkiye", "TR"},
{"turkmen", "TM"},
{"turks", "TC"},
{"tuvalu", "TV"},
{"^(?=.*\\bu\\.?\\s?s).*virgin|^(?=.*states).*virgin", "VI"},
{"uganda", "UG"},
{"ukrain", "UA"},
{"emirates|^u\\.?a\\.?e\\.?$|united.?arab.?em", "AE"},
{"united.?kingdom|britain|^u\\.?k\\.?$", "GB"},
{"united.?states\\b(?!.*islands)|\\bu\\.?s\\.?a\\.?\\b|^\\s*u\\.?s\\.?\\b(?!.*islands)", "US"},
{"minor.?outlying.?is", "UM"},
{"uruguay", "UY"},
{"uzbek", "UZ"},
{"vanuatu|new.?hebrides", "VU"},
{"holy.?see|vatican|papal.?st", "VA"},
{"venezuela", "VE"},
{"^(?!south)(?!republic).*viet.?nam(?!.*south)|democratic.republic.of.vietnam|socialist.republic.of.viet.?nam|north.viet.?nam|viet.?nam.north", "VN"},
{"futuna|wallis", "WF"},
{"western.sahara", "EH"},
{"^(?!.*arab)(?!.*north)(?!.*sana)(?!.*peo)(?!.*dem)(?!.*south)(?!.*aden)(?!.*\\bp\\.?d\\.?r).*yemen", "YE"},
{"zambia|northern.?rhodesia", "ZM"},
{"zimbabwe|^(?!.*northern).*rhodesia", "ZW"},
{"^[å|a]land", "AX"}
};

string get_country_Code(string country) {
  while(country.size() >0 && isspace(country[0]))
    country = country.substr(1);
  while (country.size() > 0 && isspace(country.back()))
    country = country.substr(0, country.size()-1);

  if (country_codes.find(country) != country_codes.end()) {
    return country_codes[country];
  }

  for (const auto& rc: country_regex) {
      boost::regex e(rc.first, boost::regex::perl|boost::regex::icase);
      if (boost::regex_search(country, e, boost::regex_constants::format_perl)) {
        return rc.second;
      }
  }
  return country;
}


// string get_country_Code(string country, Rcpp::Function countrycode) {
//   // Rcpp::String cc = countrycode(Rcpp::String(country), Rcpp::Named("origin") = Rcpp::String("country.name"), 
//   //   Rcpp::Named("destination") = Rcpp::String("iso2c"), 
//   //   Rcpp::Named("nomatch") = NA_STRING);
//   // return cc.get_cstring();

//   // Rcpp::CharacterVector cc = countrycode(country);
//   Rcpp::CharacterVector cc = countrycode(country, 
//     Rcpp::Named("origin", "country.name"), 
//     Rcpp::Named("destination", "iso2c"), 
//     Rcpp::Named("nomatch", ""));
//   string cc2 = Rcpp::as<std::string>(cc);
//   cerr << "get_country_Code " << country << " " << cc2 << endl;
//   return cc2;
// }

bool stob(std::string s, bool throw_on_error = true)
{
    auto result = false;    // failure to assert is false

    std::istringstream is(s);
    // first try simple integer conversion
    is >> result;

    if (is.fail())
    {
        // simple integer failed; try boolean
        is.clear();
        is >> std::boolalpha >> result;
    }

    if (is.fail() && throw_on_error)
    {
        throw std::invalid_argument(s.append(" is not convertable to bool"));
    }

    return result;
}

std::string to_string_with_precision(const double a_value, const int n = 6) {
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

struct Metadata {
	string id, name, date, location, location_add, pangolin;
	Metadata(string _id = "", string _name = "", string _date = "", string _location = "", string _location_add = "", string pangolin ="") :
		id(_id), name(_name), date(_date), location(_location), location_add(_location_add), pangolin(pangolin) {}
};

ostream& operator<<(ostream& os, const Metadata& m) {
	return os << "(" << m.id << "," << m.name << "," << m.date << "," << m.location << "," << m.location_add << ")";
}

vector<string> split_string(string l, char splitter = '\t') {
	// cerr << "split_string: " << l << " ... " << endl;
	vector<string> x;
	for (size_t st = 0; st != string::npos; ) {
		size_t i = l.find(splitter, st);
		if (i == string::npos) {
			x.push_back(l.substr(st));
			st = i;
		} else {
			x.push_back(l.substr(st, i - st));
			st = i + 1;
		}
	}
	// cerr << "split_string: " << l << " " << x << endl;
	return x;
}

vector<string> split_string(string l, string splitter) {
	// cerr << "split_string: " << l << " ... " << endl;
  if (splitter.size() == 0)
    throw runtime_error("Splitter size == 0");
	vector<string> x;
	for (size_t st = 0; st != string::npos; ) {
		size_t i = l.find(splitter, st);
		if (i == string::npos) {
			x.push_back(l.substr(st));
      // cerr << "f " << l.substr(st) << endl;
			st = i;
		} else {
			x.push_back(l.substr(st, i - st));
      // cerr << "f " << l.substr(st, i - st) << endl;
			st = i + splitter.size();
		}
	}
	// cerr << "split_string: " << l << ":";
  // for (const string& s : x) {
  //   cerr << s << "|";
  // }
  // cerr << endl;
	return x;
}

string trim(string str) { 
  str.erase(str.find_last_not_of(' ')+1);
  str.erase(0, str.find_first_not_of(' '));
  return str;
}

string erase_spaces(const string& s) {
  string ret = "";
  for (char c : s) {
    if (!isspace(c))
      ret += c;
  }
  return ret;
}

template<class T>
vector<T> to_vector(const set<T>& s) {
  vector<T> ret;
  for (const auto x : s) {
    ret.push_back(x);
  }
  return ret;
}


struct MetadataReader {
  string fn;
  ifstream file;

  MetadataReader(string fn) : fn(fn), file(fn) {
  }
  
  int name_index = -1, id_index = -1, date_index = -1, location_index = -1, collection_date_index = -1, location_addition_index = -1, pangolin_index=-1;
  string line = "";
  Metadata metadata;
  bool next() {
    auto indexOf = [](vector<string> str, vector<string> candids) {
      for (const string& l : candids) {
        if (std::find(str.begin(), str.end(), l) != str.end())
          return std::find(str.begin(), str.end(), l) - str.begin();
      }
      throw runtime_error("indexOf not found in the list ");
    };
    for (; line != "" || getline(file, line); ) {
      //cerr << "line read " << line << endl;
      if (line.size() == 0) {
        line = "";
        continue;
      }
      char separator = 0;
      if (separator == 0) {
        if (line.find('\t') != string::npos) 
          separator = '\t';
        else if (line.find(',') != string::npos) 
          separator = ',';
      }
      vector<string> x = split_string(trim(line), separator);
      //cerr << "line split " << x << endl;
      if (id_index == -1) {
        //cerr << "reading header " << x << endl;
        name_index = indexOf(x, {"Virus name", "Virus.name", "virus_name", "sequence_name"});
        id_index = indexOf(x, {"Accession ID", "Accession.ID", "accession_id", "sequence_name"});
        date_index = indexOf(x, {"Collection date", "Collection.date", "collection_date", "sample_date"});
        location_index = indexOf(x, {"Location", "country"});
        location_addition_index = indexOf(x, {"Additional.location.information", "Additional location information"});
        //collection_date_index = indexOf(x, {"Submission date"});
        collection_date_index = indexOf(x, {"Collection date", "Collection.date", "sample_date"});
        pangolin_index = indexOf(x, {"Pango lineage", "Pango.lineage"});
        line = "";
        //cerr << "header loaded " << x << endl;
        continue;
      } else {
        metadata = Metadata(x[id_index], x[name_index], x[date_index].size() != 10 ? x[collection_date_index] : x[date_index], x[location_index], location_addition_index != -1 ? x[location_addition_index] : "", pangolin_index != -1 ? x[pangolin_index] : "");
        line = "";
        return true;
      }
    }
    return false;
  }


};

vector<vector<string>> read_file(string fn, char delimiter = '\t') {
  vector<vector<string>> ret;
  ifstream fi(fn);
  for (string line; getline(fi, line);) {
    ret.push_back(split_string(line, delimiter));
  }
  return ret;
}

template<typename T>
vector<T> get_col(const vector<vector<T>>& i, int col) {
  vector<T> ret;
  for (const auto x: i) {
    ret.push_back(x[col]);
  }
  return ret;
}

template<class T, class S, class UnaryOperation>
std::vector<S> transform_mine(const std::vector<T>& v, UnaryOperation unary_op) {
    std::vector<S> ret;
    ret.reserve(v.size()); // Reserve space to avoid unnecessary reallocations
    std::transform(v.begin(), v.end(), std::back_inserter(ret), unary_op);
    return ret;
}

bool is_date_valid(string date) {
  return date.size() == 10 && isdigit(date[0]) && isdigit(date[1]) & isdigit(date[2]) && isdigit(date[3]) &&
    date[4] == '-' && isdigit(date[5]) && isdigit(date[6]) && date[7] == '-' && isdigit(date[8]) && isdigit(date[9]);
}

struct Acknowledgement {
  string accession_id, country, pangolin_lineage;
  Acknowledgement(string accession_id="", string country="", string pangolin_lineage=""): 
    accession_id(accession_id), country(country), pangolin_lineage(pangolin_lineage) {

  }
};


struct CountryMonthLineageReference {
  struct NameIdReference {
    map<string, int> name_to_id;
    vector<string> id_to_name;

    string name(int id) const {
      if (!(id >= 0 && (size_t) id < id_to_name.size()))
        throw runtime_error("Invalid id " + to_string(id) + " " + to_string(id_to_name.size()));
      return id_to_name[id];
    }

    int id(string name) {
      if (name_to_id.find(name) == name_to_id.end()) {
        id_to_name.push_back(name);
        name_to_id[name] = id_to_name.size() - 1;
      }
      return name_to_id[name];
    }
  };

  NameIdReference country, month, lineage;
};

struct CountryMonthLineage {
  CountryMonthLineageReference& ref;
  int country_, month_, lineage_;
  CountryMonthLineage(CountryMonthLineageReference& ref, string country, string month, string lineage) : 
    ref(ref), country_(ref.country.id(country)), month_(ref.month.id(month)), lineage_(ref.lineage.id(lineage)) {

  }

  string country() const {
    return ref.country.name(country_);
  }
  string month() const {
    return ref.month.name(month_);
  }
  string lineage() const {
    return ref.lineage.name(lineage_);
  }

  bool operator<(const CountryMonthLineage& o) const {
    if (&ref != &o.ref) return &ref < &o.ref;
    return (country_ != o.country_) ? country_ < o.country_ : (month_ != o.month_) ? month_ < o.month_ :
      lineage_ < o.lineage_;
  }
};

template<class T>
struct Matrix {
  string name;
  vector<vector<T>> data;
  vector<string> rows, cols, row_names, col_names;
  bool print_row_names, print_col_names;

  Matrix(string name, const vector<string>& rows, const vector<string>& cols, 
    std::function<T(string, string)> data_producer,
    std::function<string(string)> row_producer,
    std::function<string(string)> column_producer,
    bool print_row_names = true, bool print_col_names = true
  ) : name(name), rows(rows), cols(cols), print_row_names(print_row_names), print_col_names(print_col_names) {
    for (const auto& r : rows) {
      data.push_back({});
      for (const auto& c : cols) {
        data.back().push_back(data_producer(r, c));
      }
    }
    row_names = transform_mine<string, string, std::function<string(string)>>(rows, row_producer);
    col_names = transform_mine<string, string, std::function<string(string)>>(cols, column_producer);
  }

  void write_table(string fn, char delimiter = '\t') {
    ofstream of(fn);
    if (print_row_names && print_col_names)
      of << name << delimiter;
    if (print_col_names) {
      size_t j=0;
      for (const auto& c : col_names) {
        of << c;
        if (j+1 < col_names.size())
          of << delimiter;
        j++;
      }
      of << endl;
    }

    size_t i = 0;
    for (const auto& r : row_names) {
      if (print_row_names)
        of << r << delimiter;
      size_t j=0;
      for (const auto& c : col_names) {
        of << data[i][j];
        if (j+1 < col_names.size())
          of << delimiter;
        j++;
      }
      of << endl;
      i++;
    }
  }
};

int main(int argc, char* argv[]) {
  string file = argv[1];
  cout << "Accession ID" << "\t" << "Location" << "\t" << "Country" << "\t" << "CountryCode" << endl;
  for (MetadataReader mr(file); mr.next(); ) {
    Metadata& metadata = mr.metadata;
    vector<string> location_splitted = transform_mine<string, string, function<string(string)>>(split_string(metadata.location, " / "), [](string s) { return trim(s); });
    string country = location_splitted.size() >= 2 ? location_splitted[1] : "";
    string country_code = get_country_Code(country);
    cout << metadata.id << "\t" << metadata.location << "\t" << country << "\t" << country_code << endl;
  }
  return 0;
}
