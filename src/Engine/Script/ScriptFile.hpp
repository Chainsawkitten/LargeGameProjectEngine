#pragma once

#include <string>
#include <json/json.h>
#include "../linking.hpp"

/// Information about a file containing a script.
class ScriptFile {
    public:
    /// Saves the scriptfile to Json value.
    /**
         * @return The saved data in a JSON node.
         */
    ENGINE_API Json::Value Save() const;

    /// Loads the script information.
    /**
         * @param name Name of the script.
         */
    ENGINE_API void Load(const std::string& name);

    /// The name of the script.
    std::string name = "";

    /// The folder containing the script file.
    std::string path;
};
