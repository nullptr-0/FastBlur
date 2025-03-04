#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "EasyBMP.h"

void Blur(BMP& bitmap, short radius, bool cmp)
{
    if (radius < 1)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    int* pix = new int[w * h];
    int* oripix = new int[w * h];
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        pix[i] = (((p.Red << 8) | p.Green) << 8) | p.Blue;
        oripix[i] = pix[i];
    }
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;
    int* r = new int[wh];
    int* g = new int[wh];
    int* b = new int[wh];
    int rsum, gsum, bsum, x, y, i, p, yp, yi, yw;
    int* vmin = new int[std::max(w, h)];
    int divsum = (div + 1) >> 1;
    divsum *= divsum;
    int temp = 256 * divsum;
    int* dv = new int[temp];
    for (i = 0; i < temp; i++)
    {
        dv[i] = (i / divsum);
    }
    yw = yi = 0;
    int** stack = new int* [div];
    for (size_t i = 0; i < div; i++)
    {
        stack[i] = new int[3];
    }
    int stackpointer;
    int stackstart;
    int* sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum;
    int rinsum, ginsum, binsum;
    for (y = 0; y < h; y++)
    {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++)
        {
            p = pix[yi + std::min(wm, std::max(i, 0))];
            sir = stack[i + radius];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);
            rbs = r1 - abs(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;
        for (x = 0; x < w; x++)
        {
            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];
            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            if (y == 0)
            {
                vmin[x] = std::min(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);
            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];
            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++)
    {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++)
        {
            yi = std::max(0, yp) + x;
            sir = stack[i + radius];
            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            rbs = r1 - abs(i);
            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;
            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
            if (i < hm)
            {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++)
        {
            pix[yi] = (0xff000000 & pix[yi]) | (dv[rsum] << 16) | (dv[gsum] << 8) | dv[bsum];
            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];
            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            if (x == 0)
            {
                vmin[y] = std::min(y + r1, hm) * w;
            }
            p = x + vmin[y];
            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];
            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];
            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            yi += w;
        }
    }
    int cmpmode = -1;
    if (cmp)
    {
        srand(time(0));
        cmpmode = rand() % 8;
    }
    for (size_t i = 0; i < w * h - 1; ++i)
    {
        RGBApixel p;
        if (cmp)
        {
            switch (cmpmode)
            {
            case 0:
                if ((i % w) <= (w / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 1:
                if ((i % w) > (w / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 2:
                if ((i / w) <= (h / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 3:
                if ((i / w) > (h / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 4:
                if (((i % w) <= (w / 2) && (i / w) <= (h / 2)) || ((i % w) > (w / 2) && (i / w) > (h / 2)))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 5:
                if (!(((i % w) <= (w / 2) && (i / w) <= (h / 2)) || ((i % w) > (w / 2) && (i / w) > (h / 2))))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 6:
                if (((i % w) <= (w / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((i % w) > (w * 2 / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((w / 3) < (i % w) && (i % w) <= (w * 2 / 3) && (h / 3) < (i / w) && (i / w) <= (h * 2 / 3)))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 7:
                if (!(((i % w) <= (w / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((i % w) > (w * 2 / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((w / 3) < (i % w) && (i % w) <= (w * 2 / 3) && (h / 3) < (i / w) && (i / w) <= (h * 2 / 3))))
                {
                    pix[i] = oripix[i];
                }
                break;
            default:
                break;
            }
        }
        p.Red = pix[i] >> 16;
        p.Green = (pix[i] - (p.Red << 16)) >> 8;
        p.Blue = pix[i] - (p.Red << 16) - (p.Green << 8);
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

typedef enum {
    COLOR_RED = 1,
    COLOR_GREEN = 2,
    COLOR_BLUE = 3,
    COLOR_PURPLE = 4,
    COLOR_YELLOW = 5,
    COLOR_CYAN = 6,
    COLOR_TRANSPARENT = 0
} COLOR_FILTER;

void FilterColor(BMP& bitmap, short colorFilter)
{
    if (0 >= colorFilter || colorFilter > 6)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        if (colorFilter == COLOR_RED || colorFilter == COLOR_PURPLE || colorFilter == COLOR_YELLOW)
        {
            p.Red = 0;
        }
        if (colorFilter == COLOR_GREEN || colorFilter == COLOR_YELLOW || colorFilter == COLOR_CYAN)
        {
            p.Green = 0;
        }
        if (colorFilter == COLOR_BLUE || colorFilter == COLOR_PURPLE || colorFilter == COLOR_CYAN)
        {
            p.Blue = 0;
        }
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Brighten(BMP& bitmap, short brightnessIncrement)
{
    if (-255 > brightnessIncrement || brightnessIncrement > 255)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        int tRGB = 0;
        tRGB = p.Red + brightnessIncrement;
        if (tRGB > 255)
        {
            p.Red = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Red = 0;
            }
            else
            {
                p.Red = tRGB;
            }
        }
        tRGB = p.Green + brightnessIncrement;
        if (tRGB > 255)
        {
            p.Green = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Green = 0;
            }
            else
            {
                p.Green = tRGB;
            }
        }
        tRGB = p.Blue + brightnessIncrement;
        if (tRGB > 255)
        {
            p.Blue = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Blue = 0;
            }
            else
            {
                p.Blue = tRGB;
            }
        }
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Binarization(BMP& bitmap, int threshold, short colorFlipProbability)
{
    colorFlipProbability = 10001 - colorFlipProbability;
    if (0 > threshold || threshold > 0xffffff || colorFlipProbability <= 0 || colorFlipProbability > 10000)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    srand(time(0));
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        int pix = (((p.Red << 8) | p.Green) << 8) | p.Blue;
        if (pix >= threshold)
        {
            if (rand() % colorFlipProbability == 0)
            {
                p.Red = 0;
                p.Green = 0;
                p.Blue = 0;
            }
            else
            {
                p.Red = 255;
                p.Green = 255;
                p.Blue = 255;
            }
        }
        else
        {
            if (rand() % colorFlipProbability == 0)
            {
                p.Red = 255;
                p.Green = 255;
                p.Blue = 255;
            }
            else
            {
                p.Red = 0;
                p.Green = 0;
                p.Blue = 0;
            }
        }
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Grayed(BMP& bitmap, double gamma)
{
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        short gray = pow((pow(p.Red, gamma) * 0.2973 + pow(p.Green, gamma) * 0.6274 + pow(p.Blue, gamma) * 0.0753), (1 / gamma));
        p.Red = gray;
        p.Green = gray;
        p.Blue = gray;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

struct ProcessingOptions {
    bool do_blur = false;
    short blur_radius = 0;

    bool do_filter = false;
    COLOR_FILTER filter_color = COLOR_TRANSPARENT;

    bool do_brighten = false;
    short brighten_increment = 0;

    bool do_binarize = false;
    int binarize_threshold = 0;
    short binarize_probability = 0;

    bool do_gamma = false;
    double gamma_value = 0.0;
};

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void print_help() {
    std::cout << "Usage: path/to/program -i <input.bmp> -o <output.bmp> [options...]\n\n"
        << "Options:\n"
        << "  --blur[=radius]           Apply blur with given radius (default 0)\n"
        << "  --filter[=color]          Apply color filter (0~6, default 0)\n"
        << "                            Color filters:\n"
        << "                                red = 1\n"
        << "                                green = 2\n"
        << "                                blue = 3\n"
        << "                                purple = 4\n"
        << "                                yellow = 5\n"
        << "                                cyan = 6\n"
        << "                                transparent = 0\n"
        << "  --brighten[=increment]    Adjust brightness (-255~255, default 0)\n"
        << "  --binarize[=threshold,probability] Binarize with threshold (0~0xFFFFFF) and probability (1~10000) (defaults 0,0)\n"
        << "  --gamma[=value]           Apply gamma correction (default 0.0)\n"
        << "  -i, --input               Input BMP file\n"
        << "  -o, --output              Output BMP file\n"
        << "  --help                    Display this help message\n";
}

void process(const char* input, const char* output, const ProcessingOptions& options) {
    BMP bitmap;
    if (!bitmap.ReadFromFile(input)) {
        std::cerr << "Error: Failed to read input file." << std::endl;
        return;
    }

    if (options.do_blur) {
        Blur(bitmap, options.blur_radius, true);
    }

    if (options.do_filter) {
        FilterColor(bitmap, options.filter_color);
    }

    if (options.do_brighten) {
        Brighten(bitmap, options.brighten_increment);
    }

    if (options.do_binarize) {
        Binarization(bitmap, options.binarize_threshold, options.binarize_probability);
    }

    if (options.do_gamma) {
        Grayed(bitmap, options.gamma_value);
    }

    if (!bitmap.WriteToFile(output)) {
        std::cerr << "Error: Failed to write output file." << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string input_file;
    std::string output_file;
    ProcessingOptions options;

    for (int i = 1; i < argc; ) {
        std::string arg = argv[i];
        if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) {
                input_file = argv[i + 1];
                i += 2;
            }
            else {
                std::cerr << "Error: Missing argument for " << arg << std::endl;
                print_help();
                return 1;
            }
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i += 2;
            }
            else {
                std::cerr << "Error: Missing argument for " << arg << std::endl;
                print_help();
                return 1;
            }
        }
        else if (arg == "--help") {
            print_help();
            return 0;
        }
        else if (arg.substr(0, 2) == "--") {
            std::string opt = arg.substr(2);
            size_t eq_pos = opt.find('=');
            std::string opt_name;
            std::string opt_val;
            if (eq_pos != std::string::npos) {
                opt_name = opt.substr(0, eq_pos);
                opt_val = opt.substr(eq_pos + 1);
            }
            else {
                opt_name = opt;
            }

            if (opt_name == "blur") {
                options.do_blur = true;
                if (!opt_val.empty()) {
                    try {
                        options.blur_radius = static_cast<short>(std::stoi(opt_val));
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid blur radius." << std::endl;
                        return 1;
                    }
                    if (options.blur_radius < 0) {
                        std::cerr << "Error: Blur radius cannot be negative." << std::endl;
                        return 1;
                    }
                }
            }
            else if (opt_name == "filter") {
                options.do_filter = true;
                if (!opt_val.empty()) {
                    try {
                        short color = static_cast<short>(std::stoi(opt_val));
                        if (color < 0 || color > 6) {
                            std::cerr << "Error: Filter color must be 0~6." << std::endl;
                            return 1;
                        }
                        options.filter_color = static_cast<COLOR_FILTER>(color);
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid filter color." << std::endl;
                        return 1;
                    }
                }
            }
            else if (opt_name == "brighten") {
                options.do_brighten = true;
                if (!opt_val.empty()) {
                    try {
                        options.brighten_increment = static_cast<short>(std::stoi(opt_val));
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid brightness increment." << std::endl;
                        return 1;
                    }
                    if (options.brighten_increment < -255 || options.brighten_increment > 255) {
                        std::cerr << "Error: Brightness increment must be between -255 and 255." << std::endl;
                        return 1;
                    }
                }
            }
            else if (opt_name == "binarize") {
                options.do_binarize = true;
                std::vector<std::string> params = split(opt_val, ',');
                if (params.size() >= 1) {
                    try {
                        options.binarize_threshold = std::stoi(params[0]);
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid binarization threshold." << std::endl;
                        return 1;
                    }
                    if (options.binarize_threshold < 0 || options.binarize_threshold > 0xFFFFFF) {
                        std::cerr << "Error: Binarization threshold must be 0~0xFFFFFF." << std::endl;
                        return 1;
                    }
                }
                if (params.size() >= 2) {
                    try {
                        options.binarize_probability = static_cast<short>(std::stoi(params[1]));
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid color flip probability." << std::endl;
                        return 1;
                    }
                    if (options.binarize_probability < 0 || options.binarize_probability > 10000) {
                        std::cerr << "Error: Color flip probability must be 1~10000." << std::endl;
                        return 1;
                    }
                }
            }
            else if (opt_name == "gamma") {
                options.do_gamma = true;
                if (!opt_val.empty()) {
                    try {
                        options.gamma_value = std::stod(opt_val);
                    }
                    catch (...) {
                        std::cerr << "Error: Invalid gamma value." << std::endl;
                        return 1;
                    }
                }
            }
            else {
                std::cerr << "Error: Unknown option --" << opt_name << std::endl;
                print_help();
                return 1;
            }
            i++;
        }
        else {
            std::cerr << "Error: Unknown argument " << arg << std::endl;
            print_help();
            return 1;
        }
    }

    if (input_file.empty() || output_file.empty()) {
        std::cerr << "Error: Input and output files are required." << std::endl;
        print_help();
        return 1;
    }

    process(input_file.c_str(), output_file.c_str(), options);
    return 0;
}