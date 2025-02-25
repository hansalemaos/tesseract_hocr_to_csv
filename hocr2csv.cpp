#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#define MAX_32BIT_INT 2147483647
#define ADJUST_INDICES(start, end, len)                                                                                \
    if (end > len)                                                                                                     \
        end = len;                                                                                                     \
    else if (end < 0)                                                                                                  \
    {                                                                                                                  \
        end += len;                                                                                                    \
        if (end < 0)                                                                                                   \
            end = 0;                                                                                                   \
    }                                                                                                                  \
    if (start < 0)                                                                                                     \
    {                                                                                                                  \
        start += len;                                                                                                  \
        if (start < 0)                                                                                                 \
            start = 0;                                                                                                 \
    }

static constexpr std::pair<bool, bool> check_if_start_parsing(const std::string_view l, bool do_first_check,
                                                              bool do_second_check)
{
    std::pair<bool, bool> result{};
    size_t s = l.size();
    // If you find "<body>" at the end of the line, set result.first = true
    if ((do_first_check) && (s >= 6) && (l[s - 1] == '>') && (l[s - 2] == 'y') && (l[s - 3] == 'd') &&
        (l[s - 4] == 'o') && (l[s - 5] == 'b') && (l[s - 6] == '<'))
    {
        result.first = true;
    }
    // If you find "</body>" at the end of the line, set result.second = true
    if ((do_second_check) && (s >= 7) && (l[s - 1] == '>') && (l[s - 2] == 'y') && (l[s - 3] == 'd') &&
        (l[s - 4] == 'o') && (l[s - 5] == 'b') && (l[s - 6] == '/') && (l[s - 7] == '<'))
    {
        result.second = true;
    }
    return result;
}

static void constexpr find_all_indices__and_replace_with_char(std::string &haystack, const std::string_view needle,
                                                              char c = ' ')
{
    std::vector<size_t> indices;
    size_t offset = 0;
    size_t newoffset = 0;
    size_t maxlen = haystack.size();
    for (;;)
    {
        newoffset = haystack.substr(offset, maxlen).find(needle);
        if (newoffset == std::string::npos)
            break;
        offset += newoffset;
        indices.push_back(offset);
        maxlen = haystack.size() - offset - needle.size();
        offset += needle.size();
    }
    for (auto i : indices)
    {
        for (size_t j = 0; j < needle.size(); j++)
        {
            haystack[i + j] = c;
        }
    }
}

typedef struct line_result
{
    std::string text;
    std::string title;
    std::string id;
    std::string lang;
    std::string clazz;
    std::string tag;
    std::string bbox;
    std::string baseline;
    std::string poly;
    std::string x_bboxes;
    std::string x_font;
    std::string x_fsize;
    std::string x_size;
    std::string x_ascenders;
    std::string x_descenders;
    std::string x_wconf;
    std::string x_confs;
    std::string x_mpconf;
    std::string line_conf;
    std::string char_conf;
    std::string ppageno;
    std::string block_num;
    std::string par_num;
    std::string line_num;
    std::string word_num;
    std::string image;
    std::string scan_res;
    std::string rotate;
    std::string x_line_bboxes;
    std::string x_line_confs;
    std::string x_text;
    int64_t line_index;
    std::vector<int64_t> children;
    std::vector<int64_t> parents;
    int64_t aa_start_x;
    int64_t aa_start_y;
    int64_t aa_end_x;
    int64_t aa_end_y;
    int64_t aa_center_x;
    int64_t aa_center_y;
    int64_t aa_width;
    int64_t aa_height;
    int64_t aa_area;
    double_t aa_w_h_relation;

} line_result;

std::string static vector_to_string(std::vector<int64_t> &vec)
{
    if (vec.empty())
    {
        return "()";
    }
    std::string result;
    result.reserve(vec.size() * 3 + 2);
    result += '(';
    for (const auto &str : vec)
    {
        result += std::to_string(str) + ',';
    }
    result.pop_back()
    result += ')';
    return result;
}
static constexpr bool isspace_or_empty(const std::string_view str)
{
    if (str.size() == 0)
    {
        return true;
    }
    for (size_t i{}; i < str.size(); i++)
    {
        if (!::isspace(str[i]))
        {
            return false;
        }
    }
    return true;
}
static constexpr std::string_view delim_csv{"\",\""};
static constexpr std::string_view csv_header{
    "\"text\",\"title\",\"id\",\"lang\",\"clazz\",\"tag\",\"bbox\",\"baseline\",\"poly\",\"x_bboxes\",\"x_font\",\"x_"
    "fsize\",\"x_size\",\"x_ascenders\",\"x_descenders\",\"x_wconf\",\"x_confs\",\"x_mpconf\",\"line_conf\",\"char_"
    "conf\",\"ppageno\",\"block_num\",\"par_num\",\"line_num\",\"word_num\",\"image\",\"scan_res\",\"rotate\",\"x_line_"
    "bboxes\",\"x_line_confs\",\"x_text\",\"line_index\",\"children\",\"parents\",\"aa_start_x\",\"aa_start_y\",\"aa_"
    "end_x\",\"aa_end_y\",\"aa_center_x\",\"aa_center_y\",\"aa_width\",\"aa_height\",\"aa_area\",\"aa_w_h_"
    "relation\"\n"};

static std::string dump_struct_vector_as_csv(std::vector<line_result> &v)
{
    std::string outputstring;
    outputstring.reserve(v.size() * 256);
    outputstring.append(csv_header);
    for (line_result &it : v)
    {
        if (isspace_or_empty(it.id) && isspace_or_empty(it.title))
        {
            continue;
        }
        outputstring += '"';
        outputstring.append((it.text));
        outputstring.append(delim_csv);
        outputstring.append((it.title));
        outputstring.append(delim_csv);
        outputstring.append((it.id));
        outputstring.append(delim_csv);
        outputstring.append((it.lang));
        outputstring.append(delim_csv);
        outputstring.append((it.clazz));
        outputstring.append(delim_csv);
        outputstring.append((it.tag));
        outputstring.append(delim_csv);
        outputstring.append((it.bbox));
        outputstring.append(delim_csv);
        outputstring.append((it.baseline));
        outputstring.append(delim_csv);
        outputstring.append((it.poly));
        outputstring.append(delim_csv);
        outputstring.append((it.x_bboxes));
        outputstring.append(delim_csv);
        outputstring.append((it.x_font));
        outputstring.append(delim_csv);
        outputstring.append((it.x_fsize));
        outputstring.append(delim_csv);
        outputstring.append((it.x_size));
        outputstring.append(delim_csv);
        outputstring.append((it.x_ascenders));
        outputstring.append(delim_csv);
        outputstring.append((it.x_descenders));
        outputstring.append(delim_csv);
        outputstring.append((it.x_wconf));
        outputstring.append(delim_csv);
        outputstring.append((it.x_confs));
        outputstring.append(delim_csv);
        outputstring.append((it.x_mpconf));
        outputstring.append(delim_csv);
        outputstring.append((it.line_conf));
        outputstring.append(delim_csv);
        outputstring.append((it.char_conf));
        outputstring.append(delim_csv);
        outputstring.append((it.ppageno));
        outputstring.append(delim_csv);
        outputstring.append((it.block_num));
        outputstring.append(delim_csv);
        outputstring.append((it.par_num));
        outputstring.append(delim_csv);
        outputstring.append((it.line_num));
        outputstring.append(delim_csv);
        outputstring.append((it.word_num));
        outputstring.append(delim_csv);
        outputstring.append((it.image));
        outputstring.append(delim_csv);
        outputstring.append((it.scan_res));
        outputstring.append(delim_csv);
        outputstring.append((it.rotate));
        outputstring.append(delim_csv);
        outputstring.append((it.x_line_bboxes));
        outputstring.append(delim_csv);
        outputstring.append((it.x_line_confs));
        outputstring.append(delim_csv);
        outputstring.append((it.x_text));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.line_index));
        outputstring.append(delim_csv);
        outputstring.append(vector_to_string(it.children));
        outputstring.append(delim_csv);
        outputstring.append(vector_to_string(it.parents));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_start_x));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_start_y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_end_x));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_end_y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_center_x));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_center_y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_width));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_height));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_area));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it.aa_w_h_relation));
        outputstring.append(delim_csv);
        outputstring += '"';
        outputstring += '\n';
    }
    return outputstring;
}
constexpr static bool is_digit(char c)
{
    return c <= '9' && c >= '0';
}

constexpr static int64_t satoll_impl(const char *str, int64_t value = 0)
{
    return *str ? is_digit(*str) ? satoll_impl(str + 1, (*str - '0') + value * 10) : 0 : value;
}

int64_t static constexpr myatoll(const std::string &str)
{
    return satoll_impl(str.c_str());
}

void static constexpr calculate_bounds(line_result &mystruct, std::array<std::string, 4> &resultcoords)
{
    int64_t aa_start_x_real = myatoll(resultcoords[0]);
    int64_t aa_start_y_real = myatoll(resultcoords[1]);
    int64_t aa_end_x_real = myatoll(resultcoords[2]);
    int64_t aa_end_y_real = myatoll(resultcoords[3]);
    mystruct.aa_start_x = aa_start_x_real;
    mystruct.aa_start_y = aa_start_y_real;
    mystruct.aa_end_x = aa_end_x_real;
    mystruct.aa_end_y = aa_end_y_real;
    mystruct.aa_center_x = (mystruct.aa_end_x + mystruct.aa_start_x) / 2;
    mystruct.aa_center_y = (mystruct.aa_end_y + mystruct.aa_start_y) / 2;
    mystruct.aa_width = (mystruct.aa_end_x - mystruct.aa_start_x);
    mystruct.aa_height = (mystruct.aa_end_y - mystruct.aa_start_y);
    mystruct.aa_area = mystruct.aa_width * mystruct.aa_height;
    if (!mystruct.aa_height)
    {
        return;
    }
    mystruct.aa_w_h_relation = (double_t)(mystruct.aa_width) / (double_t)(mystruct.aa_height);
}
void static constexpr ltrim_space(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

void static constexpr rtrim_space(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

void static constexpr rtrim_space_and_semicolon(std::string &s)
{
    s.erase(
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return (!std::isspace(ch) && ch != ';'); }).base(),
        s.end());
}

static constexpr std::string_view sv_bbox{"bbox"};
static constexpr std::string_view sv_baseline{"baseline"};
static constexpr std::string_view sv_poly{"poly"};
static constexpr std::string_view sv_x_bboxes{"x_bboxes"};
static constexpr std::string_view sv_x_font{"x_font"};
static constexpr std::string_view sv_x_fsize{"x_fsize"};
static constexpr std::string_view sv_x_size{"x_size"};
static constexpr std::string_view sv_x_ascenders{"x_ascenders"};
static constexpr std::string_view sv_x_descenders{"x_descenders"};
static constexpr std::string_view sv_x_wconf{"x_wconf"};
static constexpr std::string_view sv_x_confs{"x_confs"};
static constexpr std::string_view sv_x_mpconf{"x_mpconf"};
static constexpr std::string_view sv_line_conf{"line_conf"};
static constexpr std::string_view sv_char_conf{"char_conf"};
static constexpr std::string_view sv_ppageno{"ppageno"};
static constexpr std::string_view sv_block_num{"block_num"};
static constexpr std::string_view sv_par_num{"par_num"};
static constexpr std::string_view sv_line_num{"line_num"};
static constexpr std::string_view sv_word_num{"word_num"};
static constexpr std::string_view sv_image{"image"};
static constexpr std::string_view sv_scan_res{"scan_res"};
static constexpr std::string_view sv_rotate{"rotate"};
static constexpr std::string_view sv_x_line_bboxes{"x_line_bboxes"};
static constexpr std::string_view sv_x_line_confs{"x_line_confs"};
static constexpr std::string_view sv_x_text{"x_text"};
static constexpr std::string_view sv_class{"class"};
static constexpr std::string_view sv_id{"id"};
static constexpr std::string_view sv_lang{"lang"};

static int constexpr find(const std::string_view str, const std::string_view sub, int start = 0,
                          int end = MAX_32BIT_INT)
{
    ADJUST_INDICES(start, end, (int)str.size());
    std::string::size_type result = str.substr(0, end).find(sub, start);
    if (result == std::string::npos || (result + sub.size() > (std::string::size_type)end))
    {
        return -1;
    }
    return (int)result;
}

static std::string replace(const std::string_view str, const std::string_view oldstr, const std::string_view newstr,
                           int count)
{
    int sofar{};
    int cursor{};
    std::string s{str};
    std::string::size_type oldlen{oldstr.size()};
    std::string::size_type newlen{newstr.size()};
    cursor = find(s, oldstr, cursor);
    while (cursor != -1 && cursor <= (int)s.size())
    {
        if (count > -1 && sofar >= count)
        {
            break;
        }
        s.replace(cursor, oldlen, newstr);
        cursor += (int)newlen;
        if (oldlen != 0)
        {
            cursor = find(s, oldstr, cursor);
        }
        else
        {
            ++cursor;
        }
        ++sofar;
    }
    return s;
}

static constexpr bool startswith(const std::string_view haystack, const std::string_view needle)
{
    if (haystack.size() < needle.size())
    {
        return false;
    }
    for (size_t i = 0; i < needle.size(); i++)
    {
        if (haystack[i] != needle[i])
        {
            return false;
        }
    }
    return true;
}

std::string static get_string_ready(const std::string_view ref, size_t indexstart)
{
    std::string tmpsubstr{ref.substr(indexstart)};
    find_all_indices__and_replace_with_char(tmpsubstr, "'", ' ');
    ltrim_space(tmpsubstr);
    rtrim_space(tmpsubstr);
    return tmpsubstr;
}

static constexpr std::array<char, 10> all_numbers_as_chars{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

static constexpr void set_bounds_array_to_0(std::array<std::string, 4> &boundsarray)
{

    for (int i = 0; i < 4; i++)
    {
        boundsarray[i].clear();
        boundsarray[i] += '0';
    }
}
static void constexpr parse_my_coords(const std::string_view s, std::array<std::string, 4> &resultstringarray)
{

    resultstringarray[0].clear();
    resultstringarray[1].clear();
    resultstringarray[2].clear();
    resultstringarray[3].clear();
    size_t current_idx{};
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == ' ')
        {
            current_idx++;
        }
        else
        {
            resultstringarray[current_idx] += s[i];
        }
    }
}
bool static constexpr is_rect_in_rect(const std::array<int64_t, 4> &rect1, const std::array<int64_t, 4> &rect2)
{

    return (rect1[0] >= rect2[0] && rect1[1] >= rect2[1] && rect1[2] <= rect2[2] && rect1[3] <= rect2[3]);
}
int main(int argc, char *argv[])
{
    // tesseract C:\Users\hansc\Pictures\collage_2023_04_23_13_52_30_743134.png
    // C:\Users\hansc\Pictures\collage_2023_04_23_13_52_30_743134 -l por+eng --oem 3 -c tessedit_create_hocr=1 -c
    // hocr_font_info=1
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file>" << '\n';
        return 1;
    }
    std::string my_file{argv[1]};

    std::ifstream ifs(my_file);
    if (!ifs.is_open())
    {
        std::cerr << "Error opening file: " << my_file << '\n';
        return 1;
    }

    std::string dumpstringgetline;
    dumpstringgetline.reserve(8192);
    std::vector<std::string> resultvec;
    resultvec.reserve(2048);

    bool startparsing{false};
    std::pair<bool, bool> parsingchecker{};
    bool do_first_check{true};
    bool do_second_check{false};

    size_t index_counter = 0;

    // Read lines from file
    while (std::getline(ifs, dumpstringgetline))
    {

        // Strip trailing newlines
        if (!dumpstringgetline.empty() && dumpstringgetline.back() == '\n')
        {
            dumpstringgetline.pop_back();
        }
        if (!dumpstringgetline.empty() && dumpstringgetline.back() == '\r')
        {
            dumpstringgetline.pop_back();
        }
        if (isspace_or_empty(dumpstringgetline))
        {
            continue;
        }
        // Check for <body> or </body>
        parsingchecker = check_if_start_parsing(dumpstringgetline, do_first_check, do_second_check);
        if (parsingchecker.first)
        {
            startparsing = true;
            do_first_check = false;
            do_second_check = true;
        }
        else if (parsingchecker.second)
        {
            startparsing = false;
            do_first_check = false;
            do_second_check = false;
            if (!resultvec.empty())
            {
                // remove line containing "</body>"
                resultvec.pop_back();
            }
        }

        // If inside <body>...</body>, store line + find tags
        if (startparsing)
        {
            resultvec.emplace_back(dumpstringgetline);
        }
    }
    ifs.close();

    static constexpr std::array<std::string_view, 9> split_attributes{
        "</div>", "<div>", "<div ", "</p>", "<p>", "<p ", "</span>", "<span>", "<span ",
    };
    static constexpr std::array<std::string_view, 5> new_line_at{" class='", " id='", " lang='", " title='", " text='"};
    static constexpr std::string_view sv_class_eq{"class='"};
    static constexpr std::string_view sv_id_eq{"id='"};
    static constexpr std::string_view sv_lang_eq{"lang='"};
    static constexpr std::string_view sv_title_eq{"title='"};
    static constexpr std::string_view sv_text_eq{"text='"};
    static constexpr std::string_view sv_semicol_and_space{"; "};
    std::array<std::string, 4> boundsarray{};
    boundsarray[0].reserve(6);
    boundsarray[1].reserve(6);
    boundsarray[2].reserve(6);
    boundsarray[3].reserve(6);
    std::vector<line_result> line_resultvec;
    line_resultvec.resize(resultvec.size());
    for (size_t i{}; i < resultvec.size(); i++)
    {
        for (const std::string_view sv : split_attributes)
        {
            find_all_indices__and_replace_with_char(resultvec[i], sv);
        }
        find_all_indices__and_replace_with_char(resultvec[i], "\"", '\'');
        resultvec[i] = replace(resultvec[i], ">", " text='", -1);
        for (const std::string_view sv : new_line_at)
        {
            for (;;)
            {
                size_t index_of_str{resultvec[i].find(sv)};
                if (index_of_str != std::string::npos)
                {
                    resultvec[i][index_of_str] = '\n';
                }
                else
                {
                    break;
                }
            }
        }
        auto strs{resultvec[i] | std::views::split('\n')};
        for (const auto &refr : strs)
        {
            std::string_view ref{refr.begin(), refr.end()};
            size_t index_check{ref.find(sv_class_eq)};
            if (index_check != std::string_view::npos)
            {
                line_resultvec[i].clazz = get_string_ready(ref, index_check + sv_class_eq.size());
                continue;
            }
            index_check = ref.find(sv_id_eq);
            if (index_check != std::string_view::npos)
            {
                line_resultvec[i].id = get_string_ready(ref, index_check + sv_id_eq.size());
                continue;
            }

            index_check = ref.find(sv_lang_eq);
            if (index_check != std::string_view::npos)
            {
                line_resultvec[i].lang = get_string_ready(ref, index_check + sv_lang_eq.size());
                continue;
            }

            index_check = ref.find(sv_title_eq);
            if (index_check != std::string_view::npos)
            {
                line_resultvec[i].title = get_string_ready(ref, index_check + sv_title_eq.size());
                find_all_indices__and_replace_with_char(line_resultvec[i].title, sv_semicol_and_space, '\n');
                auto strssub{line_resultvec[i].title | std::views::split('\n')};
                std::string newtmpstring;
                for (const auto &refrsub : strssub)
                {
                    std::string_view refsub{refrsub.begin(), refrsub.end()};
                    if (!isspace_or_empty(refsub))
                    {
                        newtmpstring.append(refsub);
                        newtmpstring.append("|");
                        if (startswith(refsub, sv_bbox))
                        {
                            line_resultvec[i].bbox = get_string_ready(refsub, sv_bbox.size() + 1);
                            parse_my_coords(line_resultvec[i].bbox, boundsarray);
                            calculate_bounds(line_resultvec[i], boundsarray);
                            continue;
                        }
                        if (startswith(refsub, sv_baseline))
                        {
                            line_resultvec[i].baseline = get_string_ready(refsub, sv_baseline.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_poly))
                        {
                            line_resultvec[i].poly = get_string_ready(refsub, sv_poly.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_bboxes))
                        {
                            line_resultvec[i].x_bboxes = get_string_ready(refsub, sv_x_bboxes.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_font))
                        {
                            line_resultvec[i].x_font = get_string_ready(refsub, sv_x_font.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_fsize))
                        {
                            line_resultvec[i].x_fsize = get_string_ready(refsub, sv_x_fsize.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_size))
                        {
                            line_resultvec[i].x_size = get_string_ready(refsub, sv_x_size.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_ascenders))
                        {
                            line_resultvec[i].x_ascenders = get_string_ready(refsub, sv_x_ascenders.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_descenders))
                        {
                            line_resultvec[i].x_descenders = get_string_ready(refsub, sv_x_descenders.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_wconf))
                        {
                            line_resultvec[i].x_wconf = get_string_ready(refsub, sv_x_wconf.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_confs))
                        {
                            line_resultvec[i].x_confs = get_string_ready(refsub, sv_x_confs.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_mpconf))
                        {
                            line_resultvec[i].x_mpconf = get_string_ready(refsub, sv_x_mpconf.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_line_conf))
                        {
                            line_resultvec[i].line_conf = get_string_ready(refsub, sv_line_conf.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_char_conf))
                        {
                            line_resultvec[i].char_conf = get_string_ready(refsub, sv_char_conf.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_ppageno))
                        {
                            line_resultvec[i].ppageno = get_string_ready(refsub, sv_ppageno.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_block_num))
                        {
                            line_resultvec[i].block_num = get_string_ready(refsub, sv_block_num.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_par_num))
                        {
                            line_resultvec[i].par_num = get_string_ready(refsub, sv_par_num.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_line_num))
                        {
                            line_resultvec[i].line_num = get_string_ready(refsub, sv_line_num.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_word_num))
                        {
                            line_resultvec[i].word_num = get_string_ready(refsub, sv_word_num.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_image))
                        {
                            line_resultvec[i].image = get_string_ready(refsub, sv_image.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_scan_res))
                        {
                            line_resultvec[i].scan_res = get_string_ready(refsub, sv_scan_res.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_rotate))
                        {
                            line_resultvec[i].rotate = get_string_ready(refsub, sv_rotate.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_line_bboxes))
                        {
                            line_resultvec[i].x_line_bboxes = get_string_ready(refsub, sv_x_line_bboxes.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_line_confs))
                        {
                            line_resultvec[i].x_line_confs = get_string_ready(refsub, sv_x_line_confs.size() + 1);
                            continue;
                        }
                        if (startswith(refsub, sv_x_text))
                        {
                            line_resultvec[i].x_text = get_string_ready(refsub, sv_x_text.size() + 1);
                            continue;
                        }
                    }
                }
                if (!newtmpstring.empty())
                {
                    newtmpstring.pop_back();
                }
                line_resultvec[i].title.clear();
                line_resultvec[i].title.append(newtmpstring);
                line_resultvec[i].line_index = i;
                continue;
            }

            index_check = ref.find(sv_text_eq);
            if (index_check != std::string_view::npos)
            {
                line_resultvec[i].text = get_string_ready(ref, index_check + sv_text_eq.size());
                continue;
            }
        }
    }
    std::array<int64_t, 4> rect1{0, 0, 0, 0};
    std::array<int64_t, 4> rect2{0, 0, 0, 0};
    for (size_t i{}; i < line_resultvec.size(); i++)
    {

        if ((isspace_or_empty(line_resultvec[i].id)) && (isspace_or_empty(line_resultvec[i].title)))
        {
            continue;
        }
        rect1[0] = line_resultvec[i].aa_start_x;
        rect1[1] = line_resultvec[i].aa_start_y;
        rect1[2] = line_resultvec[i].aa_end_x;
        rect1[3] = line_resultvec[i].aa_end_y;
        for (size_t j{}; j < line_resultvec.size(); j++)
        {
            if (j == i)
            {
                continue;
            }
            if (isspace_or_empty(line_resultvec[j].id) && (isspace_or_empty(line_resultvec[j].title)))
            {
                continue;
            }
            rect2[0] = line_resultvec[j].aa_start_x;
            rect2[1] = line_resultvec[j].aa_start_y;
            rect2[2] = line_resultvec[j].aa_end_x;
            rect2[3] = line_resultvec[j].aa_end_y;
            if (is_rect_in_rect(rect1, rect2))
            {
                line_resultvec[i].parents.emplace_back(line_resultvec[j].line_index);
                line_resultvec[j].children.emplace_back(line_resultvec[i].line_index);
            }
        }
    }
    std::cout << dump_struct_vector_as_csv(line_resultvec);
    return 0;
}
