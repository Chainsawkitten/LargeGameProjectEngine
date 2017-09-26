#pragma once

#include <string>
#include <json/json.h>
    
/// Information about a file containing a script.
class ScriptFile {
    public:
        /// The name of the script.
        std::string name = "";
        
        /// Saves the scriptfile to Json value.
        /**
         * @return The saved data in a JSON node.
         */
        Json::Value Save() const;
        
        /// Loads the script information.
        /**
         * @param name Name of the script.
         */
        void Load(const std::string& name);
};
