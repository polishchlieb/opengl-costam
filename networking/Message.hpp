#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <iostream>

class Message {
public:
	Message() : ownsBuffer(true) {
		size += sizeof(uint64_t);
		buffer = static_cast<char*>(malloc(bufferSize));
		currentWrite = currentRead = buffer + sizeof(size);
	}

	Message(char* buffer) : buffer(buffer), ownsBuffer(false) {
		currentRead = buffer;
		read(size);
		bufferSize = size;
		currentWrite = buffer + bufferSize;
	}

	~Message() {
		if (ownsBuffer && buffer)
			free(buffer);
	}

	template<typename T>
	void push(T value) {
		assureMemory(sizeof(value));

		memcpy(currentWrite, &value, sizeof(value));
		currentWrite += sizeof(value);

		updateSize(sizeof(value));
	}

	void pushString(const std::string& value) {
		assureMemory(value.length() + 1);

		for (uint32_t i = 0; i <= value.length(); ++i, ++currentWrite) {
			*currentWrite = value[i];
		}

		updateSize(value.length() + 1);
	}

	std::string readString() const {
		uint32_t resultSize = 1000, size = 0;
		char* result = static_cast<char*>(malloc(resultSize));

		for (uint32_t i = 0;; ++i, ++size, ++currentRead) {
			char current = *currentRead;
			if (size + 1 >= resultSize) {
				resultSize += 1000;
				char* tmp = static_cast<char*>(realloc(result, resultSize));
				if (tmp) {
					result = tmp;
				} else {
					free(buffer);
					std::cerr << "problem" << std::endl;
				}
			}

			result[i] = current;
			if (current == '\0')
				break;
		}

		return std::string{result, size};
	}

	template<typename T>
	void read(T& output) const {
		memcpy(&output, currentRead, sizeof(output));
		currentRead += sizeof(output);
	}

	uint64_t size = 0;
	char* getBuffer() {
		return buffer;
	}
	const char* getBuffer() const {
		return buffer;
	}
private:
	const bool ownsBuffer;

	uint64_t bufferSize = 1000;
	char* buffer;
	char* currentWrite = nullptr;
	mutable char* currentRead = nullptr;

	void assureMemory(uint64_t additionalBytes) {
		while (size + additionalBytes >= bufferSize) {
			bufferSize += 1000;
			char* tmp = static_cast<char*>(realloc(buffer, bufferSize));
			if (tmp) {
				buffer = tmp;
			} else {
				free(buffer);
				std::cerr << "problem" << std::endl;
			}
		}
	}

	void updateSize(uint64_t additionalBytes) {
		size += additionalBytes;
		memcpy(buffer, &size, sizeof(size));
	}
};