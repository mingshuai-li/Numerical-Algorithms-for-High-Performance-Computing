#include "util.h"
#include "auxiliary.h"
#include "boost/filesystem.hpp"
#include "external/mmio.h"
#include <boost/progress.hpp>
#include <boost/program_options.hpp>
#include <map>
#include <utility>


namespace po = boost::program_options;

std::string modeToStr(ExecutionMode mode) {
  static std::map<ExecutionMode, std::string> map{
      {ExecutionMode::CRS, "compressed row storage"},
      {ExecutionMode::ELLPACK, "ellpack"}};
  return map[mode];
}

Configuration makeConfig(int argc, char **argv) {
  Configuration config;

  po::options_description description("HPC-AA Sparse Linear Algebra Exercise.");

  std::string executionMode{};

  // clang-format off
    description.add_options()
        ("help,h", "produce help message")
        ("mode,m", po::value<std::string>(&executionMode)->default_value("crs"), "Execution modes: crs, ellpack.")
        ("matrix-file,f", po::value<std::string>(&config.matrixFile)->default_value(""), "Specify path to the Matrix the PageRank algorithm is to be executed on. Must be in MatrixMarket format.")
        ("print,p", "to print result")
        ;
  // clang-format on

  po::variables_map cmdMap;
  po::store(po::parse_command_line(argc, argv, description), cmdMap);
  po::notify(cmdMap);

  if (cmdMap.count("help")) {
    std::cout << description << std::endl;
    exit(EXIT_SUCCESS);
  }

  std::map<std::string, ExecutionMode> table{
      {"crs", ExecutionMode::CRS}, {"ellpack", ExecutionMode::ELLPACK}};

  if (table.find(executionMode) != table.end()) {
    config.executionMode = table[executionMode];
  } else {
    std::stringstream stream;
    stream << "unknown execution mode provided: " << executionMode
           << ". Allowed: ";
    for (const auto &item : table) {
      stream << item.first << ", ";
    }
    throw std::runtime_error(stream.str());
  }

  if (cmdMap.count("print")) {
    config.toPrint = true;
  } else {
    config.toPrint = false;
  }

  return config;
}

void checkConfig(const Configuration &config) {
  std::stringstream stream;
  if (config.matrixFile.empty()) {
    stream << "a matrix file needs to be provided.";
    throw std::runtime_error(stream.str());
  }
}

std::ostream &operator<<(std::ostream &stream, const Configuration &config) {
  stream << "execution mode: " << modeToStr(config.executionMode) << '\n'
         << "matrix file: " << config.matrixFile;
  return stream;
}

std::string readNextLine(FILE*& file) {
  std::string line{};
  char c{};
  while((c = static_cast<char>(std::fgetc(file))) != '\n') {
    line.push_back(c);
  }
  return line;
}

static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

std::tuple<int, int, float> parseString(std::string line) {
  std::tuple<int, int, float> result{};
  int row{-1}, col{-1};
  float value{1.0};

  rtrim(line);
  ltrim(line);

  std::string space_delimiter = " ";
  std::vector<std::string> words{};

  size_t pos = 0;
  while ((pos = line.find(space_delimiter)) != std::string::npos) {
    words.push_back(line.substr(0, pos));
    line.erase(0, pos + space_delimiter.length());
  }

  try {
    row = std::stoi(words.at(0));
    col = std::stoi(words.at(1));
    if (words.size() > 2) {
      value = std::stof(words.at(2));
    }
  }
  catch (std::exception& err) {
    std::cout << err.what() << std::endl;
    throw err;
  }
  return {row, col, value};
}


CsrMatrix loadMarketMatrix(const std::string &fileName) {
  CsrMatrix matrix;

  try {
    std::stringstream stream;

    if (!boost::filesystem::exists(fileName)) {
      stream << "File \'" << fileName << "\' doesn't exist";
      throw std::runtime_error(stream.str());
    }

    // NOTE: the file should have already been check
    FILE *file = fopen(fileName.c_str(), "r");
    if (file == NULL) {
      stream << "cannot open file: " << fileName
             << " to read. Execution is aborted";
      throw std::runtime_error(stream.str());
    }

    MM_typecode matcode;
    if (mm_read_banner(file, &matcode) != 0) {
      throw std::runtime_error("Could not process Matrix Market banner");
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */
    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
        mm_is_sparse(matcode)) {
      stream << "Sorry, this application does not support "
             << "Market Market type: " << mm_typecode_to_str(matcode);
      throw std::runtime_error(stream.str());
    }

    /* find out size of sparse matrix .... */
    int M{}, N{}, nnz{};
    int returnCode = mm_read_mtx_crd_size(file, &M, &N, &nnz);
    if (returnCode != 0) {
      stream << "failed to read matrix. Error" << returnCode;
      throw std::runtime_error(stream.str());
    }

    if (M != N) {
      stream << "Error: matrix is not square. Given, M = " << M
             << ", N = " << N;
      throw std::runtime_error(stream.str());
    }

    std::vector<int> I{};
    std::vector<int> J{};
    std::vector<float> values{};
    std::vector<int> ptr(N + 1, 0);

    if (mm_is_symmetric(matcode)) {
      nnz *= 2;

      /* reserve memory for host matrices */
      I.resize(nnz, 0);
      J.resize(nnz, 0);
      values.resize(nnz, 0.0f);

      // read non-zero matrix entries
      boost::progress_display showProgres(nnz);
      for (int i = 0; i < nnz; i += 2) {
        std::string line = readNextLine(file);
        auto [row, col, value] = parseString(line);
        I[i] = row - 1; // adjust from 1-based to 0-based
        J[i] = col - 1;

        I[i + 1] = J[i];
        J[i + 1] = I[i];
        values[i] = value;
        values[i + 1] = value;
        ++showProgres;
      }
    } else {

      /* reserve memory for host matrices */
      I.resize(nnz, 0);
      J.resize(nnz, 0);
      values.resize(nnz, 0.0f);

      // read non-zero matrix entries
      boost::progress_display showProgres(nnz);
      for (int i = 0; i < nnz; ++i) {
        std::string line = readNextLine(file);
        auto [row, col, value] = parseString(line);
        I[i] = row - 1; // adjust from 1-based to 0-based
        J[i] = col - 1;
        values[i] = value;
        ++showProgres;
      }
    }
    fclose(file);

    // write out matrix information
    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, N, N, nnz);

    matrix.numRows = N;
    matrix.nnz = nnz;
    matrix.indices = std::move(J);
    matrix.start = std::move(ptr);
    matrix.values = std::move(values);

    convertCooToCsr(matrix, I);
  } catch (std::exception &error) {
    std::stringstream stream;
    stream << "Execution aborted while reading a matrix from a file: "
           << error.what();
    throw std::runtime_error(stream.str());
  }

  return matrix;
}
