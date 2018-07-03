#include "opencv2/opencv.hpp"

static double norm(cv::Vec3b pix)
{
    return sqrt(pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
}

static cv::Mat diff_mask(const cv::Mat &image1, const cv::Mat &image2, double threshold = 30.f)
{
    cv::Mat diff, foregroundMask;

    cv::absdiff(image1, image2, diff);
    foregroundMask = cv::Mat::zeros(diff.rows, diff.cols, CV_8UC1);

    for (auto j = 0; j < diff.rows; ++j)
        for (auto i = 0; i < diff.cols; ++i)
            if (norm(diff.at<cv::Vec3b>(j,i)) > threshold)
                foregroundMask.at<unsigned char>(j,i) = 255;

    cv::cvtColor(foregroundMask, foregroundMask, cv::COLOR_GRAY2BGR);
    return foregroundMask;
}

static cv::Mat gimme_three(const cv::Mat &x, const cv::Mat &x1, const cv::Mat &x2)
{
    cv::Mat ret, xx[3] = { x1, x, x2, };
    cv::hconcat(xx, 3, ret);
    return ret;
}

static void show_diff(const cv::Mat &image1, const cv::Mat &image2, double scale, double threshold, const char *window_name)
{
    auto width_in_pixels = 1000.0; // image width in pixels for scale=1
    auto three = gimme_three(diff_mask(image1, image2, threshold), image1, image2);

    auto X = scale * width_in_pixels / three.cols;
    cv::resize(three, three, cv::Size(), X, X);

    imshow(window_name, three);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <image1> <image2>\n", argv[0]);
        exit(1);
    }

    auto image1 = cv::imread(argv[1], 1);
    auto image2 = cv::imread(argv[2], 1);

    double scale = 1.0;
    int threshold = 30;

    for ( ;; ) {

        auto window_name = argv[1];
        show_diff(image1, image2, scale, threshold, window_name);

        char c = cv::waitKey();

        if (c == '')
            break;

        else if (c == '+')
            scale *= 1.2;

        else if (c == '-')
            scale /= 1.2;

        else if (c == 't')
            threshold++;

        else if (c == 'T') {
            if (threshold > 1)
                threshold--;
        }

    }

    return 0;
}
