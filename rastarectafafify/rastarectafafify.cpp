#include "opencv2/opencv.hpp"

class State {

public:

    State(const char *window_name, const char *path);

    static void mouse_callback(int event, int x, int y, int f, void *ptr);
    void mouse_event(int event, int x, int y);

    void add_rect();
    void drop_rect();
    void print();

    int set_scale(int scale);

private:

    cv::Mat image;
    std::string window_name;

    bool mouse_clicked = false;

    cv::Point ul;
    cv::Point lr;
    cv::Rect current_box;
    std::vector<cv::Rect> boxes;

    int scale = 1;
    void adjust_size(cv::Rect &box, int M, int D);

    void print_one(const cv::Rect &r);

    void display();
};

State::State(const char *window_name, const char *path)
{
    image = cv::imread(path, 1);
    this->window_name = std::string(window_name);

    cv::namedWindow(this->window_name, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(this->window_name, State::mouse_callback, this);
}

void State::add_rect()
{
    boxes.push_back(current_box);

    current_box = cv::Rect();

    display();
}

void State::drop_rect()
{
    current_box = cv::Rect();

    display();
}

void State::print_one(const cv::Rect &r)
{
    if (!r.width || !r.height)
        return;

    std::cout <<
        r.x / scale << ", " <<
        r.y / scale << ", " <<
        (r.x + r.width) / scale << ", " <<
        (r.y + r.height) / scale << std::endl;
}

void State::print()
{
    std::cout << std::endl;

    for (auto box : boxes)
        print_one(box);

    print_one(current_box);

    std::cout << std::endl;
}

void State::display()
{
    cv::Mat tmp_image = image.clone();
    cv::resize(tmp_image, tmp_image, cv::Size(), scale, scale);

    for (auto box : boxes)
        cv::rectangle(tmp_image, box, cv::Scalar(0, 0, 255), 1, 8, 0);

    if (current_box.width && current_box.height)
        cv::rectangle(tmp_image, current_box, cv::Scalar(0, 255, 0), 1, 8, 0);

    cv::imshow(window_name, tmp_image);
}

void State::adjust_size(cv::Rect &box, int M, int D)
{
    box.x *= M; box.y *= M; box.width *= M; box.height *= M;
    box.x /= D; box.y /= D; box.width /= D; box.height /= D;
}

int State::set_scale(int scale)
{
    if (scale > 0) {

        for (auto &box : boxes)
            adjust_size(box, scale, this->scale);

        adjust_size(current_box, scale, this->scale);

        this->scale = scale;
    }

    display();

    return this->scale;
}

void State::mouse_event(int event, int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= scale * image.cols) x = scale * image.cols - 1;
    if (y >= scale * image.rows) y = scale * image.rows - 1;

    switch (event) {

    case CV_EVENT_LBUTTONDOWN:
        mouse_clicked = true;
        ul.x = lr.x = x;
        ul.y = lr.y = y;
        break;

    case  CV_EVENT_LBUTTONUP:
        lr.x = x;
        lr.y = y;
        mouse_clicked = false;
        break;

    case CV_EVENT_MOUSEMOVE:
        if (mouse_clicked) {
            lr.x = x;
            lr.y = y;
        }
        break;

    default:
        break;
    }

    if (mouse_clicked) {

        current_box.x      = (ul.x > lr.x) ? lr.x : ul.x;
        current_box.width  = (ul.x > lr.x) ? ul.x - lr.x : lr.x - ul.x;
        current_box.y      = (ul.y > lr.y) ? lr.y : ul.y;
        current_box.height = (ul.y > lr.y) ? ul.y - lr.y : lr.y - ul.y;

    }

    display();
}

void State::mouse_callback(int event, int x, int y, int f, void *ptr)
{
    State *state = (State *) ptr;
    state->mouse_event(event, x, y);
}

int main(int argc, char **argv)
{
    State state(basename(argv[0]), argv[1] ? argv[1] : "/no/such/file");

    int scale = 2;
    state.set_scale(scale);

    for ( ;; ) {

        char c = cv::waitKey();

        if (c == '')
            break;

        if (c == 'a')
            state.add_rect();

        if (c == 'd')
            state.drop_rect();

        if (c == 'p')
            state.print();

        if (c == '+')
            scale = state.set_scale(scale + 1);

        if (c == '-')
            scale = state.set_scale(scale - 1);

    }

    return 0;
}
