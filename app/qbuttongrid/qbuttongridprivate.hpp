#pragma once

#include <QPushButton>

#include <vector>


template<typename T>
class Matrix
{
    using vec = std::vector<T>;
    using mat = std::vector<vec>;

public:
    Matrix(std::size_t nCol, std::size_t nRow) : m(nCol, vec(nRow)) {}
    Matrix(std::size_t nCol, std::size_t nRow, T val) : m(nCol, vec(nRow, val)) {}
    T& itemAt(std::size_t c, std::size_t r) { return this->m.at(c).at(r); }
    const T& itemAt(std::size_t c, std::size_t r) const { return this->m.at(c).at(r); }
    std::size_t nCols() const noexcept { return this->m.size(); }
    std::size_t nRows() const noexcept { return this->m.size() > 0 ? this->m[0].size() : 0; }
    void setCell(std::size_t c, std::size_t r, T val) { this->m.at(c).at(r) = val; }
    void resize(std::size_t nCol, std::size_t nRow)
    {
        for (auto& c : this->m)
            c.resize(nRow);
        this->m.resize(nCol, vec(nRow));
    }

private:
    mat m;
};


class QButtonGridRootNode;
class QButtonGridNode
{
public:
    virtual ~QButtonGridNode() {};
    virtual bool isRoot() const = 0;
};


class QButtonGridRootNode : public QPushButton, public QButtonGridNode
{
    Q_OBJECT

public:
    QButtonGridRootNode(
        std::size_t cs = 0,
        std::size_t rs = 0,
        int id = 1)
        : QPushButton{Q_NULLPTR}
        , id{id}
        , cs{cs}
        , rs{rs}
    {
        auto sp = QSizePolicy{QSizePolicy::Preferred, QSizePolicy::Preferred};
        this->setSizePolicy(sp);
        this->setCheckable(true);
        this->setText(QString::number(id));
    }

    void setId(int id)
    {
        this->id = id;
        this->setText(QString::number(id));
    }

    bool isRoot() const override
    {
        return true;
    }

    int id;
    std::size_t cs;
    std::size_t rs;
};


class QButtonGridExtNode : public QButtonGridNode
{
public:
    QButtonGridExtNode(std::size_t dc, std::size_t dr) : dc{dc}, dr{dr} {}
    bool isRoot() const override { return false; }

    std::size_t dc;
    std::size_t dr;
};
