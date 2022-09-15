#pragma once
#ifndef CARSERV_MODEL_H
#define CARSERV_MODEL_H

template <typename... Types>
class Model {
public:
    unsigned int id;
    std::map<std::string, std::shared_ptr<BasicField>> fields = {};
    std::string tbname;
    Model() = default;
    Model(const std::map<std::string, std::shared_ptr<BasicField>>& fields);
    Model(const std::map<std::string, std::shared_ptr<BasicField>>& fields, const std::string& tbname);
    Model(Types... args);
    void create() const;
    std::vector<std::unique_ptr<Model>> read(const std::map<std::string, std::string>& data) const;
    void update(const std::map<std::string, std::string>& data);
    void remove() const;
};
template<typename... Types, size_t... Idxs>
Model<Types...> parse(std::vector<std::string> const& values, std::index_sequence<Idxs...>);
template <typename... Types>
Model<Types...> unpack(unsigned int id, std::vector<std::string> const& row);

class User : public Model<std::string, std::string, std::string> {};

#endif
