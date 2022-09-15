#pragma once
#ifndef CARSERV_MODEL_H
#define CARSERV_MODEL_H

class Model {
public:
    unsigned int id;
    std::map<std::string, std::shared_ptr<Field>> fields = {};
    std::string tbname;
    Model() = default;
    Model(const std::map<std::string, std::shared_ptr<Field>>& fields);
    Model(const std::map<std::string, std::shared_ptr<Field>>& fields, const std::string& tbname);
    void create();
    std::vector<Model*> read(const std::map<std::string, std::string>& data);
    void update(const std::map<std::string, std::string>& data);
    void remove();
};

#endif
