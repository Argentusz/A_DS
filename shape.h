#include <list>
#include <iostream>
#include <cmath>
#include "screen.h"
//==1. Поддержка экрана в форме матрицы символов ==
char screen[YMAX][XMAX];


struct errOutOfBounds {};

enum color {
    black = '*',
    white = '.'
};

void screen_init() {
    for (auto y = 0; y < YMAX; ++y)
        for (auto &x : screen[y])
            x = white;
}

void screen_destroy( )
{
    for (auto y = 0; y < YMAX; ++y)
        for (auto &x : screen[y])  x = black;
}
// проверка попадания точки на экран
bool on_screen(int a, int b) {
    return 0 <= a && a < XMAX && 0 <= b && b < YMAX;
}
void put_point(int a, int b) {
    if (on_screen(a,b))
        screen[b] [a] = black;
    else
        throw errOutOfBounds();
}
void put_line(int x0, int y0, int x1, int y1) {
    /* Алгоритм Брезенхэма для прямой:
    рисование отрезка прямой от (x0,y0) до (x1,y1).
    Уравнение прямой: b(x-x0) + a(y-y0) = 0.
    Минимизируется величина abs(eps), где eps = 2*(b(x-x0)) + a(y-y0).  */
    int dx = 1;
    int a = x1 - x0;
    if (a < 0) dx = -1, a = -a;
    int dy = 1;
    int b = y1 - y0;
    if (b < 0) dy = -1, b = -b;
    int two_a = 2*a;
    int two_b = 2*b;
    int xcrit = -b + two_a;
    int eps = 0;
    for (;;) { //Формирование прямой линии по точкам
        try{put_point(x0, y0);}
        catch (errOutOfBounds) {
            throw errOutOfBounds();
        }
        if (x0 == x1 && y0 == y1) break;
        if (eps <= xcrit) x0 += dx, eps += two_b;
        if (eps >= a || a < b) y0 += dy, eps -= two_a;
    }
}
void screen_clear( ) { screen_init( ); } //Очистка экрана
void screen_refresh( ) // Обновление экрана
{
    for (int y = YMAX-1; 0 <= y; --y) { // с верхней строки до нижней
        for (auto x : screen[y])    // от левого столбца до правого
            std::cout << x;
        std::cout << '\n';
    }
}
//== 2. Библиотека фигур ==
struct shape { // Виртуальный базовый класс "фигура"
    static std::list<shape*> shapes;// Список фигур (один на все фигуры!)
    shape( ) { shapes.push_back(this); } //Фигура присоединяется к списку
    virtual point north( ) const = 0;	//Точки для привязки
    virtual point south( ) const = 0;
    virtual point east( ) const = 0;
    virtual point west( ) const = 0;
    virtual point neast( ) const = 0;
    virtual point seast( ) const = 0;
    virtual point nwest( ) const = 0;
    virtual point swest( ) const = 0;
    virtual void draw( ) = 0;		//Рисование
    virtual void move(int, int) = 0;	//Перемещение
    virtual void resize(int) = 0;    	//Изменение размера
};
std::list<shape*> shape::shapes;   // Размещение списка фигур
void shape_refresh( ) // Перерисовка всех фигур на экране
{
    screen_clear( );
    for (auto p : shape :: shapes) p->draw( ); //Динамическое связывание!!!
    screen_refresh( );
}
class rotatable : virtual public shape { //Фигуры, пригодные к повороту
public:
    virtual void rotate_left( ) = 0;	//Повернуть влево
    virtual void rotate_right( ) = 0;	//Повернуть вправо
};
class reflectable : virtual public shape { // Фигуры, пригодные
public:					     // к зеркальному отражению
    virtual void flip_horisontally( ) = 0;	// Отразить горизонтально
    virtual void flip_vertically( ) = 0;	          // Отразить вертикально
};
class line : public shape {
/* отрезок прямой ["w", "e"].
   north( ) определяет точку "выше центра отрезка и так далеко
   на север, как самая его северная точка", и т. п. */
protected:
    point w, e;
public:
    line(point a, point b) : w(a), e(b) { };
    line(point a, int L) : w(point(a.x + L - 1, a.y)), e(a) {  };
    point north( ) const { return point((w.x+e.x)/2, e.y<w.y? w.y : e.y); }
    point south( ) const { return point((w.x+e.x)/2, e.y<w.y? e.y : w.y); }
    point east( ) const { return point(e.x<w.x? w.x : e.x, (w.y+e.y)/2); }
    point west( ) const { return point(e.x<w.x? e.x : w.x, (w.y+e.y)/2); }
    point neast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? w.y : e.y); }
    point seast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? e.y : w.y); }
    point nwest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? w.y : e.y); }
    point swest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? e.y : w.y); }
    void move(int a, int b) 	{ w.x += a; w.y += b; e.x += a; e.y += b; }
    void draw( ) { put_line(w, e); }
    void resize(int d) // Увеличение длины линии в (d) раз
    { e.x += (e.x - w.x) * (d - 1); e.y += (e.y - w.y) * (d - 1); }
};
class crossedCircle : public shape{
    /*
             ............
(upLeft)     *...****...* (upRight)
             .***....***.
             *....**....*
             *....**....*
             .***....***.
(downLeft)   *...****...* (downRight)
             ............
     */
private:
    bool isCirclePath(int x, int y, int radius) {
        if ((int)sqrt(pow(x - middle.x, 2) + pow(y - middle.y, 2)) == r) {return true; }
        return false;
    }
    void drawCircle() {
        for (auto i = 0; i < YMAX; i++) {
            for (auto j = 0; j < XMAX; j++) {
                if (isCirclePath(j, i ,r))
                    try {
                        put_point(j, i);
                    } catch (errOutOfBounds()) {
                        throw errOutOfBounds();
                    }
            }
        }
    }
protected:

public:
    point upLeft, downRight;
    crossedCircle(point up_left, point down_right) : upLeft(up_left), downRight(down_right) {
        if (upLeft.y < downRight.y) {
            std::swap(upLeft.y, downRight.y);
        }
        if (upLeft.x > downRight.y) {
            std::swap(upLeft.x, downRight.x);
        }
        downLeft = point(upLeft.x, downRight.y);
        upRight = point(downRight.x, upLeft.y);
        middle = point((int)(downRight.x + upLeft.x)/2, (int)( downRight.y + upLeft.y )/2);
        r = (int)(downRight.x-upLeft.x)/2;
    }
    point downLeft = point(upLeft.x, downRight.y);
    point upRight = point(downRight.x, upLeft.y);
    point middle = point((int)(downRight.x + upLeft.x)/2, (int)( downRight.y + upLeft.y )/2);
    int r = (int)(downRight.x-upLeft.x)/2;
    void draw() override {
        try {
            drawCircle();
            put_line(upLeft, downRight);
            put_line(upRight, downLeft);
        } catch (errOutOfBounds) {
            try {
                screen_clear();
                put_point(middle.x, middle.y);
                r = 1;
                upLeft.x = 1;    upLeft.y = 1;
                upRight.x = 1;   upRight.y = 1;
                downLeft.x = 1;  downLeft.y = 1;
                downRight.x = 1; downRight.y = 1;
            } catch (errOutOfBounds) {

            }

        }

    }
    [[nodiscard]] point north() const override { return {middle.x,upLeft.y} ; };	//Точки для привязки
    [[nodiscard]] point south() const override { return {middle.x,downLeft.y} ; };
    [[nodiscard]] point east()  const override { return {upLeft.x,middle.y} ; };
    [[nodiscard]] point west()  const override { return {upRight.x,middle.y} ; };
    [[nodiscard]] point neast() const override { return upRight; };
    [[nodiscard]] point seast() const override { return downRight; };
    [[nodiscard]] point nwest() const override { return upLeft; };
    [[nodiscard]] point swest() const override { return downLeft; };
    void move(int a, int b) override {
        if (on_screen(upLeft.x + a, upLeft.y + b) && on_screen(upRight.x + a, upRight.y + b) &&
            on_screen(downLeft.x + a, downLeft.y + b) && on_screen(downRight.x + a, downRight.y + b) &&
            on_screen(middle.x + a, middle.y + b)) {
                        upLeft.x += a;    upLeft.y += b;
                        upRight.x += a;   upRight.y += b;
                        downLeft.x += a;  downLeft.y += b;
                        downRight.x += a; downRight.y += b;
                        middle.x += a;    middle.y += b;
        } else {

                r = 1;
                upLeft.x = 1;    upLeft.y = 1;
                upRight.x = 1;   upRight.y = 1;
                downLeft.x = 1;  downLeft.y = 1;
                downRight.x = 1; downRight.y = 1;
                middle.x = 1;    middle.y = 1;
        }
    };	//Перемещение

    void resize(int) override {};    	//Изменение размера
};
// Прямоугольник
class rectangle : public rotatable {
/* nw ------ n ------ ne
   |		       |
   |		       |
   w	   c            e
   |		       |
   |		       |
   sw ------- s ------ se */
protected:
    point sw, ne;
public:
    rectangle(point a, point b) :  sw(a), ne(b) { }
    point north( ) const { return point((sw.x + ne.x) / 2, ne.y); }
    point south( ) const { return point((sw.x + ne.x) / 2, sw.y); }
    point east( ) const { return point(ne.x, (sw.y + ne.y) / 2); }
    point west( ) const { return point(sw.x, (sw.y + ne.y) / 2); }
    point neast( ) const { return ne; }
    point seast( ) const { return point(ne.x, sw.y); }
    point nwest( ) const { return point(sw.x, ne.y); }
    point swest( ) const { return sw; }
    void rotate_right() // Поворот вправо относительно se
    { int w = ne.x - sw.x, h = ne.y - sw.y; //(учитывается масштаб по осям)
        sw.x = ne.x - h * 2; ne.y = sw.y + w / 2;	}
    void rotate_left() // Поворот влево относительно sw
    { int w = ne.x - sw.x, h = ne.y - sw.y;
        ne.x = sw.x + h * 2; ne.y = sw.y + w / 2; }
    void move(int a, int b)
    { sw.x += a; sw.y += b; ne.x += a; ne.y += b; }
    void resize(int d)
    { ne.x += (ne.x - sw.x) * (d - 1); ne.y += (ne.y - sw.y) * (d - 1); }
    void draw( )
    {
        put_line(nwest( ), ne);   put_line(ne, seast( ));
        put_line(seast( ), sw);   put_line(sw, nwest( ));
    }
};
void up(shape& p, const shape& q) // поместить p над q
{	//Это ОБЫЧНАЯ функция, не член класса! Динамическое связывание!!
    point n = q.north( );
    point s = p.south( );
    p.move(n.x - s.x, n.y - s.y + 1);
}
