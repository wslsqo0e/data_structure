#ifndef __CONFIG_READER_H__
#define __CONFIG_READER_H__

#include "INIReader.h"
#include "Logger/log_spd.h"

class ConfigUnit;

class Configure
{
public:
  Configure() {
    // m_default_section_name = "";
    m_default_section_name = "GLOBAL";
    m_reader = NULL;
  }
  virtual ~Configure() {
    if (m_reader) {
      delete m_reader;
    }
  }

  int load_conf(std::string file_path);

  inline ConfigUnit& operator[](std::string name);

  inline std::string Get(std::string name, std::string default_value = "") const {
    return m_reader->Get(m_default_section_name, name, default_value);
  }

  inline long GetInteger(std::string name, long default_value = 0) const {
    return m_reader->GetInteger(m_default_section_name, name, default_value);
  }

  inline double GetReal(std::string name, double default_value = 0.) const {
    return m_reader->GetReal(m_default_section_name, name, default_value);
  }

  inline bool GetBoolean(std::string name, bool default_value = false) const {
    return m_reader->GetBoolean(m_default_section_name, name, default_value);
  }

  inline std::vector<std::string> GetArray(std::string name) const {
    std::string str = m_reader->Get(m_default_section_name, name);
    std::vector<std::string> ret;
    if (str.size() == 0) {
      return ret;
    }
    size_t pos = 0;
    while (true) {
      size_t n_pos = str.find("\n", pos);
      if (n_pos == std::string::npos) {
        break;
      }
      std::string part = str.substr(pos, n_pos - pos);
      ret.push_back(part);
      pos = n_pos + 1;
    }
    std::string part = str.substr(pos);
    ret.push_back(part);
    return ret;
  }
private:
  INIReader* m_reader;
  std::string m_default_section_name;
  std::map<std::string, ConfigUnit> _units;
};

class ConfigUnit : public Configure
{
public:
  ConfigUnit() {};
  ConfigUnit(const INIUnit& unit, std::string sec_name, Configure* reader) : m_reader_ptr(reader) {
    m_unit = unit;
    m_section_name = sec_name;
  }
  ConfigUnit(const ConfigUnit& other) {
    m_unit = other.m_unit;
    m_section_name = other.m_section_name;
    m_reader_ptr = other.m_reader_ptr;
  }
  ConfigUnit& operator = (const ConfigUnit& other) {
    m_unit = other.m_unit;
    m_section_name = other.m_section_name;
    m_reader_ptr = other.m_reader_ptr;
    return *this;
  }

  // 使得通过 ConfigUnit 获取其它 ConfigUnit 可能
  inline ConfigUnit& operator[](std::string name) const {
    return (*m_reader_ptr)[name];
  }

  inline std::string Get(std::string name, std::string default_value = "") const {
    return m_unit.Get(name, default_value);
  }

  inline long GetInteger(std::string name, long default_value = 0) const {
    return m_unit.GetInteger(name, default_value);
  }

  inline double GetReal(std::string name, double default_value = 0.) const {
    return m_unit.GetReal(name, default_value);
  }

  inline bool GetBoolean(std::string name, bool default_value = false) const {
    return m_unit.GetBoolean(name, default_value);
  }

  inline std::vector<std::string> GetArray(std::string name) const {
    std::string str = m_unit.Get(name);
    std::vector<std::string> ret;
    if (str.size() == 0) {
      return ret;
    }
    size_t pos = 0;
    while (true) {
      size_t n_pos = str.find("\n", pos);
      if (n_pos == std::string::npos) {
        break;
      }
      std::string part = str.substr(pos, n_pos - pos);
      ret.push_back(part);
      pos = n_pos + 1;
    }
    std::string part = str.substr(pos);
    ret.push_back(part);
    return ret;
  }

  inline std::string get_section_name() const {
    return m_section_name;
  }
private:
  INIUnit m_unit;
  std::string m_section_name;
  Configure* m_reader_ptr;
};

inline int Configure::load_conf(std::string file_path) {
  m_reader = new INIReader(file_path);
  if (!(*m_reader)) {
    delete m_reader;
    m_reader = NULL;
    return -1;
  }

  for (auto s_name : m_reader->Sections()) {
    _units[s_name] = ConfigUnit((*m_reader)[s_name], s_name, this);
  }
  return 0;
}

inline ConfigUnit& Configure::operator[](std::string name) {
  if (!_units.count(name)) {
    LOG_CRITICAL("config file has no section: {}", name);
  }
  return _units[name];
}

#endif
