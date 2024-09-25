# Define variables for compiler, source file, and flags
CXX = g++
CXXFLAGS = -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=1
LIBS = -lmysqlcppconnx  # Use mysqlcppconnx for MySQLX

# Platform-specific flags
ifeq ($(shell uname), Darwin)
    # macOS
    CXXFLAGS += -I/opt/homebrew/Cellar/mysql-connector-c++/9.0.0/include
    LDFLAGS = -L/opt/homebrew/lib
else
    # Linux (Fedora)
    CXXFLAGS += -I/usr/include/mysql-cppconn/mysqlx
    LDFLAGS = -L/usr/lib64
endif

TARGET = reminder_app
SRC = reminder_app.cpp

.PHONY: all clean

# Default rule to build the project
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean up the compiled files
clean:
	rm -f $(TARGET)
