# Define variables for compiler, source file, and flags
CXX = g++
CXXFLAGS = -I/usr/include/mysql-cppconn/jdbc -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=1
LIBS = -lmysqlcppconn
TARGET = reminder_app
SRC = reminder_app.cpp

.PHONY: all clean

# Default rule to build the project
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up the compiled files
clean:
	rm -f $(TARGET)
