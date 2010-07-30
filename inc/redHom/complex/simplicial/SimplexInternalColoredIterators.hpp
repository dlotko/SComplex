//#ifndef COLOREDITERATORS_HPP
// #define COLOREDITERATORS_HPP

struct ColoredIterators
{
    struct Iterators
    {
        const int color;
        SimplexSComplex &comp;

        Iterators(SimplexSComplex &cmplx, int c) : color(c), comp(cmplx)
        {
        }

        struct DimCells
        {
            typedef Simplex::colored_iterator iterator;

            SimplexSComplex &comp;
            int color;
            int dim;

            explicit DimCells(SimplexSComplex &cmplx, int col, int d) : comp(cmplx), color(col), dim(d) {}

            iterator begin()
            {
                return comp.dim_begin(dim, color);
            }

            iterator end()
            {
                return comp.dim_end(dim, color);
            }
        };

        DimCells dimCells(int dim)
        {
            return DimCells(comp, color, dim);
        }

        struct BdCells
        {
            typedef Simplex::border_iterator iterator;
            typedef Simplex::const_border_iterator const_iterator;

            SimplexSComplex &comp;
            const Simplex &cell; // ?? really const ??
            int color;

            explicit BdCells(SimplexSComplex &cmplx, const Simplex &c, int col) : comp(cmplx), cell(c), color(col) {}

            iterator begin()
            {
                return const_cast<Simplex&>(cell).border_begin(color);
            }

            iterator end()
            {
                return const_cast<Simplex&>(cell).border_end(color);
            }

            const_iterator begin() const
            {
                return cell.border_begin(color);
            }

            const_iterator end() const
            {
                return cell.border_end(color);
            }
        };

        template<typename ImplT>
        BdCells bdCells(const CellProxy<ImplT> &c)
        {
            return BdCells(comp, *c.getImpl(), color);
        }

        struct AllCells
        {
            typedef Simplex::colored_iterator iterator;
            SimplexSComplex &comp;
            int color;

            explicit AllCells(SimplexSComplex &cmplx, int col) : comp(cmplx), color(col) {}

            iterator begin()
            {
                return comp.all_begin(color);
            }

            iterator end()
            {
                return comp.all_end(color);
            }
        };

        AllCells allCells()
        {
            return AllCells(comp, color);
        }

        struct CbdCells
        {
            typedef Simplex::coborder_iterator iterator;
            typedef Simplex::const_coborder_iterator const_iterator;

            SimplexSComplex &comp;
            const Simplex &cell; // ?? really const ??
            int color;

            explicit CbdCells(SimplexSComplex &cmplx, const Simplex &c, int col) : comp(cmplx), cell(c), color(col) {}

            iterator begin()
            {
                return const_cast<Simplex&>(cell).coborder_begin(color);
            }

            iterator end()
            {
                return const_cast<Simplex&>(cell).coborder_end(color);
            }

            const_iterator begin() const
            {
                return cell.coborder_begin(color);
            }

            const_iterator end() const
            {
                return cell.coborder_end(color);
            }
        };

        template<typename ImplT>
        CbdCells cbdCells(const CellProxy<ImplT> &c)
        {
            return CbdCells(comp, *c.getImpl(), color);
        }
    };
};

struct Iterators
{
    const int color;
    SimplexSComplex &comp;

    Iterators(SimplexSComplex &cmplx, int c = -1) : color(c), comp(cmplx)
    {
    }

    struct DimCells
    {
        typedef Simplex::colored_iterator iterator;

        SimplexSComplex &comp;
        int color;
        int dim;

        explicit DimCells(SimplexSComplex &cmplx, int col, int d) : comp(cmplx), color(col), dim(d) {}

        iterator begin()
        {
            return comp.dim_begin(dim, color);
        }

        iterator end()
        {
            return comp.dim_end(dim, color);
        }
    };

    DimCells dimCells(int dim)
    {
        return DimCells(comp, color, dim);
    }

    struct BdCells
    {
        typedef Simplex::border_iterator iterator;
        typedef Simplex::const_border_iterator const_iterator;

        SimplexSComplex &comp;
        const Simplex &cell; // ?? really const ??
        int color;

        explicit BdCells(SimplexSComplex &cmplx, const Simplex &c, int col) : comp(cmplx), cell(c), color(col) {}

        iterator begin()
        {
            return const_cast<Simplex&>(cell).border_begin(color);
        }

        iterator end()
        {
            return const_cast<Simplex&>(cell).border_end(color);
        }

        const_iterator begin() const
        {
            return cell.border_begin(color);
        }

        const_iterator end() const
        {
            return cell.border_end(color);
        }
    };

    template<typename ImplT>
    BdCells bdCells(const CellProxy<ImplT> &c)
    {
        return BdCells(comp, *c.getImpl(), color);
    }

    struct AllCells
    {
        typedef Simplex::colored_iterator iterator;
        SimplexSComplex &comp;
        int color;

        explicit AllCells(SimplexSComplex &cmplx, int col) : comp(cmplx), color(col) {}

        iterator begin()
        {
            return comp.all_begin(color);
        }

        iterator end()
        {
            return comp.all_end(color);
        }
    };

    AllCells allCells()
    {
        return AllCells(comp, color);
    }

    struct CbdCells
    {
        typedef Simplex::coborder_iterator iterator;
        typedef Simplex::const_coborder_iterator const_iterator;

        SimplexSComplex &comp;
        const Simplex &cell; // ?? really const ??
        int color;

        explicit CbdCells(SimplexSComplex &cmplx, const Simplex &c, int col) : comp(cmplx), cell(c), color(col) {}

        iterator begin()
        {
            return const_cast<Simplex&>(cell).coborder_begin(color);
        }

        iterator end()
        {
            return const_cast<Simplex&>(cell).coborder_end(color);
        }

        const_iterator begin() const
        {
            return cell.coborder_begin(color);
        }

        const_iterator end() const
        {
            return cell.coborder_end(color);
        }
    };

    template<typename ImplT>
    CbdCells cbdCells(const CellProxy<ImplT> &c)
    {
        return CbdCells(comp, *c.getImpl(), color);
    }
};


// #endif
