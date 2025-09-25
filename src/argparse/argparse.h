#ifndef LUMOS_ARGPARSE_H_
#define LUMOS_ARGPARSE_H_

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace lumos {
namespace argparse {

// Exception types for argument parsing errors
class ArgumentError : public std::runtime_error {
public:
    explicit ArgumentError(const std::string& message) 
        : std::runtime_error("ArgumentError: " + message) {}
};

class ArgumentTypeError : public ArgumentError {
public:
    explicit ArgumentTypeError(const std::string& message)
        : ArgumentError("Type conversion error: " + message) {}
};

// Forward declaration
class ArgumentParser;

// Base class for argument values
class ArgumentValue {
public:
    virtual ~ArgumentValue() = default;
    virtual std::string asString() const = 0;
    virtual bool asBool() const = 0;
    virtual int asInt() const = 0;
    virtual double asDouble() const = 0;
    virtual std::vector<std::string> asStringList() const = 0;
    virtual bool isSet() const = 0;
};

// Template implementation for different value types
template<typename T>
class TypedArgumentValue : public ArgumentValue {
public:
    explicit TypedArgumentValue(const T& value) : value_(value), is_set_(true) {}
    TypedArgumentValue() : is_set_(false) {}

    const T& get() const { return value_; }
    void set(const T& value) { value_ = value; is_set_ = true; }
    bool isSet() const override { return is_set_; }

    std::string asString() const override {
        if (!is_set_) throw ArgumentError("Argument not set");
        return toString(value_);
    }

    bool asBool() const override {
        if (!is_set_) throw ArgumentError("Argument not set");
        return toBool(value_);
    }

    int asInt() const override {
        if (!is_set_) throw ArgumentError("Argument not set");
        return toInt(value_);
    }

    double asDouble() const override {
        if (!is_set_) throw ArgumentError("Argument not set");
        return toDouble(value_);
    }

    std::vector<std::string> asStringList() const override {
        if (!is_set_) throw ArgumentError("Argument not set");
        return toStringList(value_);
    }

private:
    T value_;
    bool is_set_;

    // Type conversion helpers
    std::string toString(const std::string& val) const { return val; }
    std::string toString(int val) const { return std::to_string(val); }
    std::string toString(double val) const { return std::to_string(val); }
    std::string toString(bool val) const { return val ? "true" : "false"; }
    std::string toString(const std::vector<std::string>& val) const {
        std::string result = "[";
        for (size_t i = 0; i < val.size(); ++i) {
            if (i > 0) result += ", ";
            result += val[i];
        }
        result += "]";
        return result;
    }

    bool toBool(const std::string& val) const {
        return val == "true" || val == "1" || val == "yes" || val == "on";
    }
    bool toBool(bool val) const { return val; }
    bool toBool(int val) const { return val != 0; }
    bool toBool(double val) const { return val != 0.0; }
    bool toBool(const std::vector<std::string>& val) const { return !val.empty(); }

    int toInt(const std::string& val) const {
        try {
            return std::stoi(val);
        } catch (const std::exception& e) {
            throw ArgumentTypeError("Cannot convert '" + val + "' to int");
        }
    }
    int toInt(int val) const { return val; }
    int toInt(bool val) const { return val ? 1 : 0; }
    int toInt(double val) const { return static_cast<int>(val); }
    int toInt(const std::vector<std::string>& val) const { 
        throw ArgumentTypeError("Cannot convert string list to int");
    }

    double toDouble(const std::string& val) const {
        try {
            return std::stod(val);
        } catch (const std::exception& e) {
            throw ArgumentTypeError("Cannot convert '" + val + "' to double");
        }
    }
    double toDouble(double val) const { return val; }
    double toDouble(int val) const { return static_cast<double>(val); }
    double toDouble(bool val) const { return val ? 1.0 : 0.0; }
    double toDouble(const std::vector<std::string>& val) const {
        throw ArgumentTypeError("Cannot convert string list to double");
    }

    std::vector<std::string> toStringList(const std::vector<std::string>& val) const {
        return val;
    }
    std::vector<std::string> toStringList(const std::string& val) const {
        return {val};
    }
    std::vector<std::string> toStringList(int val) const {
        return {std::to_string(val)};
    }
    std::vector<std::string> toStringList(double val) const {
        return {std::to_string(val)};
    }
    std::vector<std::string> toStringList(bool val) const {
        return {val ? "true" : "false"};
    }
};

// Argument definition class
class Argument {
public:
    enum class Type {
        STRING,
        INT,
        DOUBLE,
        BOOL,
        STRING_LIST
    };

    Argument(const std::string& name, Type type = Type::STRING)
        : name_(name), type_(type), required_(false), positional_(false), 
          takes_value_(true), action_store_true_(false) {}

    // Builder pattern methods
    Argument& help(const std::string& help_text) {
        help_ = help_text;
        return *this;
    }

    Argument& required(bool is_required = true) {
        required_ = is_required;
        return *this;
    }

    Argument& defaultValue(const std::string& default_val) {
        default_value_ = default_val;
        return *this;
    }

    Argument& choices(const std::vector<std::string>& valid_choices) {
        choices_ = valid_choices;
        return *this;
    }

    Argument& action(const std::string& action_type) {
        if (action_type == "store_true") {
            action_store_true_ = true;
            takes_value_ = false;
            type_ = Type::BOOL;
        }
        return *this;
    }

    Argument& nargs(const std::string& n) {
        nargs_ = n;
        if (n == "+" || n == "*") {
            type_ = Type::STRING_LIST;
        }
        return *this;
    }

    // Getters
    const std::string& getName() const { return name_; }
    Type getType() const { return type_; }
    bool isRequired() const { return required_; }
    bool isPositional() const { return positional_; }
    bool takesValue() const { return takes_value_; }
    bool isStoreTrue() const { return action_store_true_; }
    const std::string& getHelp() const { return help_; }
    const std::string& getDefaultValue() const { return default_value_; }
    const std::vector<std::string>& getChoices() const { return choices_; }
    const std::string& getNargs() const { return nargs_; }

    // Internal setters
    void setPositional(bool pos) { positional_ = pos; }

    // Validation
    bool validateChoice(const std::string& value) const {
        if (choices_.empty()) return true;
        return std::find(choices_.begin(), choices_.end(), value) != choices_.end();
    }

private:
    std::string name_;
    Type type_;
    bool required_;
    bool positional_;
    bool takes_value_;
    bool action_store_true_;
    std::string help_;
    std::string default_value_;
    std::vector<std::string> choices_;
    std::string nargs_;
};

// Main ArgumentParser class
class ArgumentParser {
public:
    explicit ArgumentParser(const std::string& description = "")
        : description_(description), prog_name_("program") {}

    // Add arguments
    Argument& addArgument(const std::string& name, Argument::Type type = Argument::Type::STRING) {
        auto arg = std::make_shared<Argument>(name, type);
        
        if (name.empty() || (name[0] != '-' && name.find("--") != 0)) {
            // Positional argument
            arg->setPositional(true);
            positional_args_.push_back(arg);
        } else {
            // Optional argument
            optional_args_[name] = arg;
            
            // Handle short forms (-h) and long forms (--help)
            if (name.size() > 2 && name.substr(0, 2) == "--") {
                std::string short_form = "-" + name.substr(2, 1);
                if (optional_args_.find(short_form) == optional_args_.end()) {
                    optional_args_[short_form] = arg;
                }
            }
        }
        
        return *arg;
    }

    // Parse arguments
    void parseArgs(int argc, char* argv[]) {
        prog_name_ = argv[0];
        
        // Initialize all arguments
        initializeArguments();
        
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        parseArgumentList(args);
        validateRequiredArguments();
    }

    void parseArgs(const std::vector<std::string>& args) {
        initializeArguments();
        parseArgumentList(args);
        validateRequiredArguments();
    }

    // Get parsed values
    template<typename T>
    T get(const std::string& name) const {
        auto it = parsed_values_.find(getCanonicalName(name));
        if (it == parsed_values_.end()) {
            throw ArgumentError("Argument '" + name + "' not found");
        }
        
        auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<T>>(it->second);
        if (!typed_value) {
            throw ArgumentTypeError("Type mismatch for argument '" + name + "'");
        }
        
        return typed_value->get();
    }

    // Check if argument was provided
    bool isSet(const std::string& name) const {
        auto it = parsed_values_.find(getCanonicalName(name));
        return it != parsed_values_.end() && it->second->isSet();
    }

    // Generate help text
    std::string formatHelp() const {
        std::ostringstream help;
        
        help << "Usage: " << prog_name_;
        
        // Add optional arguments to usage
        if (!optional_args_.empty()) {
            help << " [options]";
        }
        
        // Add positional arguments to usage
        for (const auto& arg : positional_args_) {
            help << " " << arg->getName();
        }
        
        help << "\n\n";
        
        if (!description_.empty()) {
            help << description_ << "\n\n";
        }
        
        // Positional arguments
        if (!positional_args_.empty()) {
            help << "Positional arguments:\n";
            for (const auto& arg : positional_args_) {
                help << "  " << arg->getName();
                if (!arg->getHelp().empty()) {
                    help << "    " << arg->getHelp();
                }
                help << "\n";
            }
            help << "\n";
        }
        
        // Optional arguments
        if (!optional_args_.empty()) {
            help << "Optional arguments:\n";
            
            // Add help option
            help << "  -h, --help    Show this help message and exit\n";
            
            for (const auto& pair : optional_args_) {
                if (pair.first.substr(0, 2) == "--") {  // Only show long forms
                    const auto& arg = pair.second;
                    help << "  " << pair.first;
                    
                    // Show type hint
                    if (arg->takesValue()) {
                        switch (arg->getType()) {
                            case Argument::Type::INT:
                                help << " INT";
                                break;
                            case Argument::Type::DOUBLE:
                                help << " FLOAT";
                                break;
                            case Argument::Type::STRING_LIST:
                                help << " [STRING ...]";
                                break;
                            default:
                                help << " STRING";
                                break;
                        }
                    }
                    
                    if (!arg->getHelp().empty()) {
                        help << "    " << arg->getHelp();
                    }
                    
                    if (!arg->getDefaultValue().empty()) {
                        help << " (default: " << arg->getDefaultValue() << ")";
                    }
                    
                    help << "\n";
                }
            }
        }
        
        return help.str();
    }

    void printHelp() const {
        std::cout << formatHelp();
    }

private:
    std::string description_;
    std::string prog_name_;
    std::vector<std::shared_ptr<Argument>> positional_args_;
    std::map<std::string, std::shared_ptr<Argument>> optional_args_;
    std::map<std::string, std::shared_ptr<ArgumentValue>> parsed_values_;

    void initializeArguments() {
        parsed_values_.clear();
        
        // Initialize all arguments with default values
        for (const auto& arg : positional_args_) {
            initializeArgument(arg);
        }
        
        for (const auto& pair : optional_args_) {
            if (pair.first.substr(0, 2) == "--") {  // Only initialize long forms
                initializeArgument(pair.second);
            }
        }
    }

    void initializeArgument(std::shared_ptr<Argument> arg) {
        std::string canonical_name = getCanonicalName(arg->getName());
        
        switch (arg->getType()) {
            case Argument::Type::STRING:
                if (!arg->getDefaultValue().empty()) {
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<std::string>>(arg->getDefaultValue());
                } else {
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<std::string>>();
                }
                break;
            case Argument::Type::INT:
                if (!arg->getDefaultValue().empty()) {
                    int default_val = std::stoi(arg->getDefaultValue());
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<int>>(default_val);
                } else {
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<int>>();
                }
                break;
            case Argument::Type::DOUBLE:
                if (!arg->getDefaultValue().empty()) {
                    double default_val = std::stod(arg->getDefaultValue());
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<double>>(default_val);
                } else {
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<double>>();
                }
                break;
            case Argument::Type::BOOL:
                if (arg->isStoreTrue()) {
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<bool>>(false);
                } else {
                    bool default_val = !arg->getDefaultValue().empty() && 
                                     (arg->getDefaultValue() == "true" || arg->getDefaultValue() == "1");
                    parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<bool>>(default_val);
                }
                break;
            case Argument::Type::STRING_LIST:
                parsed_values_[canonical_name] = std::make_shared<TypedArgumentValue<std::vector<std::string>>>();
                break;
        }
    }

    void parseArgumentList(const std::vector<std::string>& args) {
        size_t pos_arg_index = 0;
        
        for (size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            
            // Check for help
            if (arg == "-h" || arg == "--help") {
                printHelp();
                std::exit(0);
            }
            
            // Optional argument
            if (arg[0] == '-') {
                auto it = optional_args_.find(arg);
                if (it == optional_args_.end()) {
                    throw ArgumentError("Unknown argument: " + arg);
                }
                
                auto argument = it->second;
                std::string canonical_name = getCanonicalName(argument->getName());
                
                if (argument->isStoreTrue()) {
                    auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<bool>>(parsed_values_[canonical_name]);
                    typed_value->set(true);
                } else if (argument->takesValue()) {
                    if (i + 1 >= args.size()) {
                        throw ArgumentError("Argument " + arg + " requires a value");
                    }
                    
                    ++i;  // Move to the value
                    const std::string& value = args[i];
                    
                    if (!argument->validateChoice(value)) {
                        throw ArgumentError("Invalid choice '" + value + "' for argument " + arg);
                    }
                    
                    setArgumentValue(canonical_name, argument, value);
                }
            } else {
                // Positional argument
                if (pos_arg_index >= positional_args_.size()) {
                    throw ArgumentError("Too many positional arguments");
                }
                
                auto argument = positional_args_[pos_arg_index];
                std::string canonical_name = getCanonicalName(argument->getName());
                
                if (!argument->validateChoice(arg)) {
                    throw ArgumentError("Invalid choice '" + arg + "' for argument " + argument->getName());
                }
                
                setArgumentValue(canonical_name, argument, arg);
                ++pos_arg_index;
            }
        }
    }

    void setArgumentValue(const std::string& canonical_name, std::shared_ptr<Argument> argument, const std::string& value) {
        switch (argument->getType()) {
            case Argument::Type::STRING: {
                auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<std::string>>(parsed_values_[canonical_name]);
                typed_value->set(value);
                break;
            }
            case Argument::Type::INT: {
                try {
                    int int_value = std::stoi(value);
                    auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<int>>(parsed_values_[canonical_name]);
                    typed_value->set(int_value);
                } catch (const std::exception& e) {
                    throw ArgumentTypeError("Cannot convert '" + value + "' to integer");
                }
                break;
            }
            case Argument::Type::DOUBLE: {
                try {
                    double double_value = std::stod(value);
                    auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<double>>(parsed_values_[canonical_name]);
                    typed_value->set(double_value);
                } catch (const std::exception& e) {
                    throw ArgumentTypeError("Cannot convert '" + value + "' to double");
                }
                break;
            }
            case Argument::Type::BOOL: {
                bool bool_value = (value == "true" || value == "1" || value == "yes" || value == "on");
                auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<bool>>(parsed_values_[canonical_name]);
                typed_value->set(bool_value);
                break;
            }
            case Argument::Type::STRING_LIST: {
                auto typed_value = std::dynamic_pointer_cast<TypedArgumentValue<std::vector<std::string>>>(parsed_values_[canonical_name]);
                std::vector<std::string> current_list;
                if (typed_value->isSet()) {
                    current_list = typed_value->get();
                }
                current_list.push_back(value);
                typed_value->set(current_list);
                break;
            }
        }
    }

    void validateRequiredArguments() {
        // Check required positional arguments
        for (size_t i = 0; i < positional_args_.size(); ++i) {
            auto arg = positional_args_[i];
            std::string canonical_name = getCanonicalName(arg->getName());
            
            if (arg->isRequired() && !parsed_values_[canonical_name]->isSet()) {
                throw ArgumentError("Missing required positional argument: " + arg->getName());
            }
        }
        
        // Check required optional arguments
        for (const auto& pair : optional_args_) {
            if (pair.first.substr(0, 2) == "--") {  // Only check long forms
                auto arg = pair.second;
                std::string canonical_name = getCanonicalName(arg->getName());
                
                if (arg->isRequired() && !parsed_values_[canonical_name]->isSet()) {
                    throw ArgumentError("Missing required argument: " + pair.first);
                }
            }
        }
    }

    std::string getCanonicalName(const std::string& name) const {
        if (name.substr(0, 2) == "--") {
            return name.substr(2);  // Remove --
        } else if (name[0] == '-') {
            return name.substr(1);   // Remove -
        }
        return name;  // Positional argument
    }
};

} // namespace argparse
} // namespace lumos

#endif // LUMOS_ARGPARSE_H_