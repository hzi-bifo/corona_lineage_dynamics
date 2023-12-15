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
  // {"Bonaire","BQ"},
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
  // {"Crimea","UA"},
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
  // {"Kosovo","XK"},
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
  // {"Canary Islands", "CI"}
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
      string r = "";
      for (const string& l : candids)
        r += " " + l;
      throw runtime_error("indexOf not found in the list " + r);
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
        name_index = indexOf(x, {"Virus name", "Virus.name", "virus_name", "sequence_name", "virus"});
        id_index = indexOf(x, {"Accession ID", "Accession.ID", "accession_id", "sequence_name", "gisaid_epi_isl"});
        date_index = indexOf(x, {"Collection date", "Collection.date", "collection_date", "sample_date", "date"});
        location_index = indexOf(x, {"Location", "country"});
        location_addition_index = indexOf(x, {"Additional.location.information", "Additional location information"});
        //collection_date_index = indexOf(x, {"Submission date"});
        collection_date_index = indexOf(x, {"Collection date", "Collection.date", "sample_date"});
        pangolin_index = indexOf(x, {"Pango lineage", "Pango.lineage", "pangolin_lineage", "Lineage"});
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

// [[Rcpp::export]]
// int count_lineages_country(std::string file, std::string months_file, std::string output_folder, 
//   bool corrected, std::string alias_file, std::string lineage_date_file,
//   Rcpp::Function countrycode) {
int main(int argc, char* argv[]) {
  // Command line arguments
  // cerr << "currently it is " << argc << endl;
  // for (size_t i=0; i<argc; i++) {
  //   cerr << argv[i] << ":";
  // }
  // cerr << endl;

  // if (argc < 7) {
  //     std::cerr << "Usage: " << argv[0] << " file months_file output_folder corrected alias_file lineage_date_file\n";
  //     return 1;
  // }

  std::string file(argv[1]);
  std::string months_file(argv[2]);
  std::string output_folder(argv[3]);
  bool corrected = stob(argv[4]);
  std::string alias_file(argv[5]);
  std::string lineage_date_file(argv[6]);
  // int country_sample_size_threshold = argc > 7 ? stoi(argv[7]) : 2000;
  int country_sample_size_threshold = 2000;

  vector<string> months = get_col(read_file(months_file), 0);
  vector<vector<string>> alias = read_file(alias_file);
  vector<pair<string,string>> alias_x = transform_mine<vector<string>, pair<string,string>, function<pair<string,string>(vector<string>)>>(alias, [](const vector<string>& v) -> std::pair<std::string, std::string> { return make_pair(v[0], v[1]);});
  map<string, string> to_alias(alias_x.begin(), alias_x.end());

  vector<pair<string,string>> unalias_x = transform_mine<vector<string>, pair<string,string>, function<pair<string,string>(vector<string>)>>(alias, [](const vector<string>& v) -> std::pair<std::string, std::string> { return make_pair(v[1], v[0]);});
  map<string, string> to_unalias(unalias_x.begin(), unalias_x.end());

  vector<vector<string>> lineage_dates = read_file(lineage_date_file);
  vector<pair<string,string>> x = transform_mine<vector<string>, pair<string,string>, function<pair<string,string>(vector<string>)>>(lineage_dates, [](const vector<string>& v) -> std::pair<std::string, std::string> { return make_pair(v[0], v[1]);});
  map<string, string> lineage_date_map(x.begin(), x.end());

  CountryMonthLineageReference country_month_lineage_reference;
  map<CountryMonthLineage,int> all_data_counts,
    nseqs;

  // map<CountryMonthLineage,vector<string>> all_data_ids;

  set<string> all_lineages;
  map<string, int> country_count;
  // map<string, vector<Acknowledgement>> country_acknowledgements;

  // lineages present in country, we use it to mimic the R code, as the R code
  // if corrected=true, only adds samples to super pango-lineages which are present
  // in that country.
  map<string, set<string>> country_lineages;

  ofstream fo_ack(output_folder + "/acknowledgements.tsv");
  fo_ack << "Accession_ID" << "\t" << "country" << "\t" << "pangolin_lineage" << endl;

  for (MetadataReader mr(file); mr.next(); ) {
    Metadata& metadata = mr.metadata;
    vector<string> location_splitted = transform_mine<string, string, function<string(string)>>(split_string(metadata.location, " / "), [](string s) { return trim(s); });
    string country = location_splitted.size() >= 2 ? location_splitted[1] : "";
    string country_code = get_country_Code(country);

    string date_first = lineage_date_map.find(metadata.pangolin) != lineage_date_map.end() ? lineage_date_map[metadata.pangolin] : "";
    if (metadata.date < date_first && date_first != "" && is_date_valid(metadata.date) && is_date_valid(date_first)) {
      // cerr << "Invalid date: " << metadata.date << " " << date_first << " " << is_date_valid(metadata.date) << " " << is_date_valid(date_first) << endl;
      continue;
    }

    // if (metadata.id == "EPI_ISL_2295488") {
    //   cerr << "after date check" << endl;
    // }
    
    if (metadata.pangolin.find('/') != string::npos)
      continue;

    if (to_alias.find(metadata.pangolin) != to_alias.end())
      metadata.pangolin = to_alias[metadata.pangolin];
    
    country_count[country_code]++;
    // country_acknowledgements[country_code].push_back(Acknowledgement(metadata.id, country, metadata.pangolin));
    fo_ack << metadata.id << "\t" << country << "\t" << metadata.pangolin << endl;


    if (metadata.pangolin == "NA" || metadata.pangolin == "") {
      continue;
    }

    if (metadata.pangolin == "" || metadata.pangolin == "None")
      continue;

    all_lineages.insert(metadata.pangolin);
    country_lineages[country_code].insert(metadata.pangolin);

    // if (metadata.id == "EPI_ISL_2295488") {
    //   cerr << "added to country_lineages" << endl;
    // }

    // as the tibble loads incomplete date as NA, it will not included in actual counts,
    //   but it will be present in country_lineages
    // TODO: the following line should work only if first 1000 samples all are in a date format, otherwise read_tsv reads Collection date as a string.
    // if (!is_date_valid(metadata.date)) {
    //   continue;
    // }

    string month = metadata.date.substr(0, 7);
    // if (corrected == false) {
    all_data_counts[CountryMonthLineage(country_month_lineage_reference, country_code, month, metadata.pangolin)]++;
    // if (country_code == "BE" && month == "2022-03" && metadata.pangolin == "B.1.1.529.2.9") {
    //   cerr << metadata << endl;
    // }
    // all_data_ids[CountryMonthLineage(country_month_lineage_reference, country_code, month, metadata.pangolin)].push_back(metadata.id);
    // } else {
    //   for (string p = metadata.pangolin; p != ""; p = p.find('.')== string::npos ? "" : p.substr(0, p.rfind('.'))) {
    //     if (p == "B.1.1.28" && country_code == "US") {
    //       cerr << "DEBUG " << p << " " << country_code << " " << metadata.pangolin << " " << metadata.id << endl;
    //     }
    //     all_data_counts[CountryMonthLineage(country_month_lineage_reference, country_code, month, p)]++;
    //   }
    // }
    nseqs[CountryMonthLineage(country_month_lineage_reference, country_code, month, "")]++;
  }

  if (corrected == true) {
    // The R code's count values are highly dependent on the country, so we need these codes to mimic it.
    map<CountryMonthLineage,int> all_data_counts_old = all_data_counts;
    all_data_counts.clear();

    for (const auto& cml_c: all_data_counts_old) {
      string month = cml_c.first.month(),
        country = cml_c.first.country();

      for (string p = cml_c.first.lineage(); p != ""; p = p.find('.')== string::npos ? "" : p.substr(0, p.rfind('.'))) {
        
        // if (p == "B.1.1.1" && country == "US") {
        //   cerr << "DEBUG " << p << " " << country << " " << cml_c.first.lineage() << " " << (country_lineages[country].find(p) != country_lineages[country].end()) << endl;
        // }

        if (country_lineages[country].find(p) != country_lineages[country].end()) {
          all_data_counts[CountryMonthLineage(country_month_lineage_reference, country, month, p)] += cml_c.second;

        }
      }
    }
  }

  // it seems if corrected = true, the sum is sum of rows, but as one sample may include in more than one lineage
  map<CountryMonthLineage, int> all_data_counts_sum = all_data_counts;

  // map<CountryMonthLineage, double> all_data_freqs;
  for (const auto& x : all_data_counts) {
    // all_data_freqs[x.first] = x.second * 1.0 / nseqs[CountryMonthLineage(x.first.country, x.first.month, "")];
    all_data_counts_sum[CountryMonthLineage(country_month_lineage_reference, x.first.country(), x.first.month(), "all")] += x.second;
  }

  // map<string, int> all_lineage_count;
  // for (const auto& x : all_data_counts) {
  //   if (country_count[x.first.country()] <= country_sample_size_threshold)
  //     continue;
  //   all_lineage_count[x.first.lineage()] += x.second;
  // }
  // vector<string> all_lineages_vector(all_lineages.begin(), all_lineages.end());
  // all_lineages_vector.erase(remove_if(all_lineages_vector.begin(), all_lineages_vector.end(), [&all_lineage_count](const string& l) { return all_lineage_count[l] == 0; }));
  // all_lineages = set(all_lineages_vector.begin(), all_lineages_vector.end());
  
  set<string> country_selected;
  for (const auto& cc: country_count) {
    string country_name = cc.first;
    if (cc.second <= country_sample_size_threshold) {
      // cerr << "Country with not enough samples " << country_name << " " << cc.second << "<=" << country_sample_size_threshold << endl;
      continue;
    }
    country_selected.insert(country_name);

    vector<string> all_lineages_old(all_lineages.begin(), all_lineages.end());
    set<string> all_lineages;
    for (const auto& l : all_lineages_old) {
      bool is_lineage_present_in_country = false;
      for (const auto& m: months) {
        is_lineage_present_in_country |= 
          all_data_counts_sum[CountryMonthLineage(country_month_lineage_reference, country_name, m, l)] > 0;
      }
      if (is_lineage_present_in_country)
        all_lineages.insert(l);
    }
    
    // std::string::iterator end_pos = std::remove(country_name.begin(), country_name.end(), ' ');
    // country_name.erase(end_pos, country_name.end());
    string country_name_ns = erase_spaces(country_name);
    string output_path = output_folder + "/" + country_name_ns;
    std::filesystem::create_directory(output_path);

    string output = output_path + "/" + country_name_ns + (corrected == false ? "" : "_corrected");
    vector<string> all_lineage_names = to_vector(all_lineages);
    all_lineage_names.insert(all_lineage_names.begin(), "all");

    Matrix<int> all_data_counts_sum_matrix("lineage", 
      all_lineage_names, months, 
      [&all_data_counts_sum, &country_name, &country_month_lineage_reference](const string& l, const string& m) { 
        return (all_data_counts_sum.find(CountryMonthLineage(country_month_lineage_reference, country_name, m, l)) != 
          all_data_counts_sum.end()) ? 
            all_data_counts_sum[CountryMonthLineage(country_month_lineage_reference, country_name, m, l)] : 
            0; 
      },
      [&to_unalias](const string& l) { return to_unalias.find(l) != to_unalias.end() ? to_unalias[l] : l; },
      [](const string& m) { return m;});
    all_data_counts_sum_matrix.write_table(output + "_counts.txt", '\t');


    Matrix<string> all_data_freqs_matrix("lineage",
      to_vector(all_lineages), months, 
      [&all_data_counts, &country_name, &nseqs, &country_month_lineage_reference](const string& l, const string& m) { 
        return (nseqs.find(CountryMonthLineage(country_month_lineage_reference, country_name, m, "")) != nseqs.end() && 
                nseqs[CountryMonthLineage(country_month_lineage_reference, country_name, m, "")] != 0)?
          to_string_with_precision(all_data_counts[CountryMonthLineage(country_month_lineage_reference, country_name, m, l)] * 1.0 / nseqs[CountryMonthLineage(country_month_lineage_reference, country_name, m, "")], 20) :
          "NA";
        // return (all_data_freqs.find(CountryMonthLineage(country_name, m, l)) != all_data_freqs.end()) ? 
        //   to_string(all_data_freqs[CountryMonthLineage(country_name, m, l)]) : "NA"; 
      },
      [&to_unalias](const string& l) { return to_unalias.find(l) != to_unalias.end() ? to_unalias[l] : l; },
      [](const string& m) { return m;});
    all_data_freqs_matrix.write_table(output + "_frequencies.txt", '\t');

    Matrix<int> months_matrix("", 
      months, vector<string>(),
      [](const string& r, const string& c) { return 0; },
      [](const string& r) { return r; },
      [](const string& c) { return c; },
      true, false
    );
    months_matrix.write_table(output + ".months.txt");

    Matrix<int> nseqs_matrix("", 
      vector<string>{""}, months,
      [&nseqs, &country_name, &country_month_lineage_reference](const string& r, const string& c) { return nseqs[CountryMonthLineage(country_month_lineage_reference, country_name, c, "")]; },
      [](const string& r) { return r; },
      [](const string& c) { return c; },
      false, false
    );
    nseqs_matrix.write_table(output + ".numIsolates.txt", ' ');

    Matrix<int> all_lineages_alias_matrix("", 
      vector<string>{}, to_vector(all_lineages),
      [](const string& r, const string& c) { return 0; },
      [](const string& r) { return r; },
      [&to_unalias](const string& l) { return to_unalias.find(l) != to_unalias.end() ? to_unalias[l] : l; },
      false, true
    );
    all_lineages_alias_matrix.write_table(output + ".mutations.txt");

    //without the all row
    Matrix<int> all_data_counts_matrix("lineage", 
      vector<string>(all_lineage_names.begin()+1, all_lineage_names.end()), months, 
      [&all_data_counts_sum, &country_name, &country_month_lineage_reference](const string& l, const string& m) { 
        return (all_data_counts_sum.find(CountryMonthLineage(country_month_lineage_reference, country_name, m, l)) != all_data_counts_sum.end()) ? all_data_counts_sum[CountryMonthLineage(country_month_lineage_reference, country_name, m, l)] : 0; },
      [&to_unalias](const string& l) { return to_unalias.find(l) != to_unalias.end() ? to_unalias[l] : l; },
      [](const string& m) { return m;}, false, false);
    all_data_counts_matrix.write_table(output + ".subtreeMutationMap.txt", '\t');

  }

  // for (const auto& cc: country_count) {
  //   string country = cc.first;
  //   for (const auto& ack : country_acknowledgements[country]) {
  //     fo_ack << ack.accession_id << "\t" << ack.country << "\t" << ack.pangolin_lineage << endl;
  //   }

  // }
  return 0;
}

// # rename column
// colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Pango lineage"] = "pangolin_lineage"
// colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Collection date"] = "collection_date"
// colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Accession ID"] = "Accession_ID"

// # get countries with more than 2k sequences
// numbers <- table(gisaid_data_all$country_code)
// countries <- names(numbers[numbers > 2000])

// #acknowledgements <- data.frame(Accession.ID = character(), country = character(), pangolin_lineage = character(), stringAsFactors = FALSE) #, originating_lab = character(), submitting_lab = character(), stringsAsFactors = FALSE)
// acknowledgements <- subset(gisaid_data_all, select=c("Accession_ID", "country", "pangolin_lineage"))

// # count lineages for each countries > 2k
// for (c in 1:length(countries)){
//   # get data subset for the current country
//   gisaid_data <- gisaid_data_all[gisaid_data_all$country_code == countries[c],]
//   acknowledgements <- rbind(acknowledgements, gisaid_data[c("Accession_ID", "country", "pangolin_lineage")])
//   all_lineages <- sort(unique(gisaid_data$pangolin_lineage))
//   all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]
  
//   # get alias to write in output
//   all_lineages_alias <- all_lineages
//   for (i in 1:nrow(alias)){
//     all_lineages_alias[all_lineages_alias == alias[i,2]] <- alias[i,1]
//   }
//   all_data_counts <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)
//   all_data_freqs <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)
  
//   # set timeframe that should be calculated (alternative: read from file)
//   #months <- c("2020-01", "2020-02", "2020-03", "2020-04", "2020-05", "2020-06", "2020-07", "2020-08", "2020-09", "2020-10", "2020-11", "2020-12", "2021-01")
//   n_seqs <- vector(mode = "integer", length = length(months))
  
//   # loop over months and lineages to calculate counts and frequencies
//   for (m in 1:length(months)){
//     gisaid_data_subset <- gisaid_data[grep(months[m], gisaid_data$collection_date),]
//     n_seq <- nrow(gisaid_data_subset)
//     n_seqs[m] <- n_seq
//     for (i in 1:length(all_lineages)){
//       if (corrected == FALSE){
//         # count only lineage,  without sublineages
//         count <- sum(gisaid_data_subset$pangolin_lineage == all_lineages[i])
//       } else if (corrected == TRUE){
//         # count lineage with sublineages
//         count <- sum(gisaid_data_subset$pangolin_lineage == all_lineages[i] | startsWith(gisaid_data_subset$pangolin_lineage, paste(all_lineages[i], ".", sep = "")))
//       }
//       all_data_counts[i, 1+m] <- count
//       all_data_freqs[i, 1+m] <- count/n_seq
//     }
//   }
  
//   # set names in dataframe
//   names(all_data_counts) <- c("lineage", months)
//   names(all_data_freqs) <- c("lineage", months)
  
//   # rename aliases
//   all_data_counts[,1] <- all_lineages_alias
//   all_data_freqs[,1] <- all_lineages_alias
  
//   # add total counts
//   all_data_counts_sum <- rbind(c("all", t(colSums(all_data_counts[,2:ncol(all_data_counts)]))), all_data_counts)
  
//   # for creating folder and filenames: country without spaces
//   country_name <- gsub(" ", "", countries[c], fixed = TRUE)
//   output_path <- paste(output_folder, "/", country_name, sep = "")
//   dir.create(output_path)

//   if (corrected == FALSE){
//     output <- paste(output_path, "/", country_name, sep = "")
//   } else if (corrected == TRUE){
//     output <- paste(output_path, "/", country_name, "_corrected", sep = "")
//   }

//   # write tables
//   write.table(all_data_counts_sum, paste(output, "_counts.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
//   write.table(all_data_freqs, paste(output, "_frequencies.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
  
//   ### write output as input for SD plot evaluation&visualization
  
//   # months actually used in the analysis
//   write.table(months, paste(output, ".months.txt", sep = ""), quote = FALSE, row.names = FALSE, col.names = FALSE)
  
//   # numIsolates.txt
//   # number of seqs per timeframe, tab separated
//   sink(paste(output, ".numIsolates.txt", sep = ""))
//   cat(paste(n_seqs, collapse = " "))
//   cat("\n")
//   sink()
  
//   # mutations.per_position.txt
//   # mutations/labels for frequencies; tab separated
//   sink(paste(output, ".mutations.txt", sep = ""))
//   cat(paste(all_lineages_alias, collapse = "\t"))
//   cat("\n")
//   sink()
  
//   # subtreeMutationMap.per_position.txt
//   # counts with time in columns and mutations in rows
//   write.table(all_data_counts[,-1], paste(output, ".subtreeMutationMap.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = FALSE)
// }

// #acknowledgements_aggregated <- aggregate(data = acknowledgements, gisaid_epi_isl~originating_lab + submitting_lab + authors, paste, collapse = ", ")
// write.table(acknowledgements, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
// #write.table(acknowledgements_aggregated, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
