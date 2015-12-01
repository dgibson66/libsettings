/*
 * Copyright (c) 2014, Nic McDonald, Stanford University
 * See LICENSE file for details
 */
#include <jsoncpp/json/json.h>
#include <gtest/gtest.h>

#include <string>

#include "settings/Settings.h"

const char* JSON =
    "{\n"
    "  \"name\": \"Nic\",\n"
    "  \"age\": 30,\n"
    "  \"family\": {\n"
    "    \"wife\": {\n"
    "      \"name\": \"Kara\",\n"
    "      \"age\": 27\n"
    "    },\n"
    "    \"kids\": [\n"
    "      {\n"
    "      \"name\": \"Kaylee\",\n"
    "      \"age\": 3\n"
    "      },\n"
    "      {\n"
    "      \"name\": \"Ruby\",\n"
    "      \"age\": 0\n"
    "      }\n"
    "    ]\n"
    "  }\n"
    "}\n";

void Settings_TEST(const Json::Value& settings) {
  bool debug = false;
  if (debug) {
    printf("%s\n", settings::Settings::toString(settings).c_str());
  }

  ASSERT_EQ(settings.size(), 3u);

  Json::Value name = settings["name"];
  ASSERT_EQ(name.size(), 0u);
  ASSERT_EQ(name.asString(), "Nic");

  Json::Value age = settings["age"];
  ASSERT_EQ(age.size(), 0u);
  ASSERT_EQ(age.asUInt(), 30u);

  Json::Value family = settings["family"];
  ASSERT_EQ(family.size(), 2u);

  Json::Value wife = family["wife"];
  ASSERT_EQ(wife.size(), 2u);
  ASSERT_EQ(wife["name"].asString(), "Kara");
  ASSERT_EQ(wife["age"].asUInt(), 27u);

  Json::Value kids = family["kids"];
  ASSERT_EQ(kids.size(), 2u);

  Json::Value kid0 = kids[0];
  ASSERT_EQ(kid0.size(), 2u);
  ASSERT_EQ(kid0["name"].asString(), "Kaylee");
  ASSERT_EQ(kid0["age"].asUInt(), 3u);

  Json::Value kid1 = kids[1];
  ASSERT_EQ(kid1.size(), 2u);
  ASSERT_EQ(kid1["name"].asString(), "Ruby");
  ASSERT_EQ(kid1["age"].asUInt(), 0u);
}

TEST(Settings, string) {
  Json::Value settings;
  settings::Settings::initString(JSON, &settings);

  Settings_TEST(settings);
}

TEST(Settings, file) {
  const char* filename = "TEST_settings.json";
  FILE* fp = fopen(filename, "w");
  assert(fp != NULL);
  fprintf(fp, "%s", JSON);
  fclose(fp);

  Json::Value settings;
  settings::Settings::initFile(filename, &settings);

  Settings_TEST(settings);

  assert(remove(filename) == 0);
}

TEST(Settings, update) {
  Json::Value settings;
  settings::Settings::initString(JSON, &settings);

  Settings_TEST(settings);

  std::vector<std::string> updates = {
    "family.kids[0].name=string=Krazy"
  };
  settings::Settings::update(&settings, updates);

  Json::Value kid0 = settings["family"]["kids"][0];
  ASSERT_EQ(kid0.size(), 2u);
  ASSERT_EQ(kid0["name"].asString(), "Krazy");
  ASSERT_EQ(kid0["age"].asUInt(), 3u);

  Json::Value wife = settings["family"]["wife"];
  ASSERT_EQ(wife.size(), 2u);
  ASSERT_EQ(wife["name"].asString(), "Kara");
  ASSERT_EQ(wife["age"].asUInt(), 27u);
  ASSERT_FALSE(wife.isMember("sexy"));

  updates = {
    "family.wife.sexy=bool=true"
  };
  settings::Settings::update(&settings, updates);

  wife = settings["family"]["wife"];
  ASSERT_EQ(wife.size(), 3u);
  ASSERT_EQ(wife["name"].asString(), "Kara");
  ASSERT_EQ(wife["age"].asUInt(), 27u);
  ASSERT_TRUE(wife.isMember("sexy"));
  ASSERT_EQ(wife["sexy"].asBool(), true);
}
