#pragma once
#ifndef CARSERV_MODEL_H
#define CARSERV_MODEL_H

template <typename... Types>
class Model {
    friend std::ostream& operator << (std::ostream& out, Model model);
private:
    Model(Types... args);  // Parser-aimed constructor
public:
    unsigned int id;
    std::map<std::string, std::shared_ptr<BasicField>> fields = {};
    std::string tbname;
    Model(  // Value-init constructor
        const std::map<std::string, std::shared_ptr<BasicField>>& fields
    ): fields(std::move(fields)) {};
    Model(  // Table-init constructor
        const std::map<std::string, std::shared_ptr<BasicField>>& fields,
        const std::string& tbname
    );
    // CRUD methods
    void create() const;
    std::vector<std::unique_ptr<Model>> read(const std::map<std::string, std::string>& data) const;
    void update(const std::map<std::string, std::string>& data);
    void remove() const;
};
template<typename... Types, size_t... Idxs>
Model<Types...> parse(std::vector<std::string> const& values, std::index_sequence<Idxs...>);
template <typename... Types>
Model<Types...> unpack(unsigned int id, std::vector<std::string> const& row);

template <typename... Types>
std::ostream& operator << (std::ostream& out, Model<Types...> model);

#endif
