/*
SaltyNES Copyright (c) 2012 Matthew Brennan Jones <matthew.brennan.jones@gmail.com>
vNES Copyright (c) 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "SaltyNES.h"



ByteBuffer::ByteBuffer(int size, int byteOrdering) {
	if(size < 1) {
		size = 1;
	}
	this->buf = new vector<int16_t>(size);
	this->size = size;
	this->byteOrder = byteOrdering;
	curPos = 0;
	hasBeenErrors = false;
	this->byteOrder = BO_BIG_ENDIAN;
	this->expandable = true;
	this->expandBy = 4096;
}

ByteBuffer::ByteBuffer(vector<int8_t>* content, int byteOrdering) {
	try {
		this->buf = new vector<int16_t>(size);
		for(size_t i = 0; i < content->size(); i++) {
			(*buf)[i] = (int16_t) ((*content)[i] & 255);
		}
		size = content->size();
		this->byteOrder = byteOrdering;
		curPos = 0;
		hasBeenErrors = false;
	} catch (exception& e) {
		//System.out.println("ByteBuffer: Couldn't create buffer from empty array.");
	}
}

void ByteBuffer::setExpandable(bool exp) {
	expandable = exp;
}

void ByteBuffer::setExpandBy(int expBy) {

	if(expBy > 1024) {
		this->expandBy = expBy;
	}

}

void ByteBuffer::setByteOrder(int byteOrder) {

	if(byteOrder >= 0 && byteOrder < 2) {
		this->byteOrder = byteOrder;
	}

}

int8_t* ByteBuffer::getBytes() {
	int8_t* ret = new int8_t[buf->size()];
	for(size_t i = 0; i < buf->size(); i++) {
		ret[i] = (int8_t) (*buf)[i];
	}
	return ret;
}

int ByteBuffer::getSize() {
	return this->size;
}

int ByteBuffer::getPos() {
	return curPos;
}

void ByteBuffer::error() {
	hasBeenErrors = true;
//System.out.println("Not in range!");
}

bool ByteBuffer::hasHadErrors() {
	return hasBeenErrors;
}

void ByteBuffer::clear() {
	for(size_t i = 0; i < buf->size(); i++) {
		(*buf)[i] = 0;
	}
	curPos = 0;
}

void ByteBuffer::fill(int8_t value) {
	for(int i = 0; i < size; i++) {
		(*buf)[i] = value;
	}
}

bool ByteBuffer::fillRange(int start, int length, int8_t value) {
	if(inRange(start, length)) {
		for(int i = start; i < (start + length); i++) {
			(*buf)[i] = value;
		}
		return true;
	} else {
		error();
		return false;
	}
}

void ByteBuffer::resize(int length) {
	buf->resize(size);
	size = length;
}

void ByteBuffer::resizeToCurrentPos() {
	resize(curPos);
}

void ByteBuffer::expand() {
	expand(expandBy);
}

void ByteBuffer::expand(int byHowMuch) {
	resize(size + byHowMuch);
}

void ByteBuffer::goTo(int position) {
	if(inRange(position)) {
		curPos = position;
	} else {
		error();
	}
}

void ByteBuffer::move(int howFar) {
	curPos += howFar;
	if(!inRange(curPos)) {
		curPos = size - 1;
	}
}

bool ByteBuffer::inRange(int pos) {
	if(pos >= 0 && pos < size) {
		return true;
	} else {
		if(expandable) {
			expand(max(pos + 1 - size, expandBy));
			return true;
		} else {
			return false;
		}
	}
}

bool ByteBuffer::inRange(int pos, int length) {
	if(pos >= 0 && pos + (length - 1) < size) {
		return true;
	} else {
		if(expandable) {
			expand(max(pos + length - size, expandBy));
			return true;
		} else {
			return false;
		}
	}
}

bool ByteBuffer::putBoolean(bool b) {
	bool ret = putBoolean(b, curPos);
	move(1);
	return ret;
}

bool ByteBuffer::putBoolean(bool b, int pos) {
	if(b) {
		return putByte((int16_t) 1, pos);
	} else {
		return putByte((int16_t) 0, pos);
	}
}

bool ByteBuffer::putByte(int16_t var) {
	if(inRange(curPos, 1)) {
		(*buf)[curPos] = var;
		move(1);
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putByte(int16_t var, int pos) {
	if(inRange(pos, 1)) {
		(*buf)[pos] = var;
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putShort(int16_t var) {
	bool ret = putShort(var, curPos);
	if(ret) {
		move(2);
	}
	return ret;
}

bool ByteBuffer::putShort(int16_t var, int pos) {
	if(inRange(pos, 2)) {
		if(this->byteOrder == BO_BIG_ENDIAN) {
			(*buf)[pos + 0] = (int16_t) ((var >> 8) & 255);
			(*buf)[pos + 1] = (int16_t) ((var) & 255);
		} else {
			(*buf)[pos + 1] = (int16_t) ((var >> 8) & 255);
			(*buf)[pos + 0] = (int16_t) ((var) & 255);
		}
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putInt(int var) {
	bool ret = putInt(var, curPos);
	if(ret) {
		move(4);
	}
	return ret;
}

bool ByteBuffer::putInt(int var, int pos) {
	if(inRange(pos, 4)) {
		if(this->byteOrder == BO_BIG_ENDIAN) {
			(*buf)[pos + 0] = (int16_t) ((var >> 24) & 255);
			(*buf)[pos + 1] = (int16_t) ((var >> 16) & 255);
			(*buf)[pos + 2] = (int16_t) ((var >> 8) & 255);
			(*buf)[pos + 3] = (int16_t) ((var) & 255);
		} else {
			(*buf)[pos + 3] = (int16_t) ((var >> 24) & 255);
			(*buf)[pos + 2] = (int16_t) ((var >> 16) & 255);
			(*buf)[pos + 1] = (int16_t) ((var >> 8) & 255);
			(*buf)[pos + 0] = (int16_t) ((var) & 255);
		}
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putString(string var) {
	bool ret = putString(var, curPos);
	if(ret) {
		move(2 * var.length());
	}
	return ret;
}

bool ByteBuffer::putString(string var, int pos) {
	char* charArr = (char*) var.c_str();
	int16_t theChar;
	if(inRange(pos, var.length() * 2)) {
		for(size_t i = 0; i < var.length(); i++) {
			theChar = (int16_t) (charArr[i]);
			(*buf)[pos + 0] = (int16_t) ((theChar >> 8) & 255);
			(*buf)[pos + 1] = (int16_t) ((theChar) & 255);
			pos += 2;
		}
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putChar(char var) {
	bool ret = putChar(var, curPos);
	if(ret) {
		move(2);
	}
	return ret;
}

bool ByteBuffer::putChar(char var, int pos) {
	int tmp = var;
	if(inRange(pos, 2)) {
		if(byteOrder == BO_BIG_ENDIAN) {
			(*buf)[pos + 0] = (int16_t) ((tmp >> 8) & 255);
			(*buf)[pos + 1] = (int16_t) ((tmp) & 255);
		} else {
			(*buf)[pos + 1] = (int16_t) ((tmp >> 8) & 255);
			(*buf)[pos + 0] = (int16_t) ((tmp) & 255);
		}
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putCharAscii(char var) {
	bool ret = putCharAscii(var, curPos);
	if(ret) {
		move(1);
	}
	return ret;
}

bool ByteBuffer::putCharAscii(char var, int pos) {
	if(inRange(pos)) {
		(*buf)[pos] = (int16_t) var;
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putStringAscii(string var) {
	bool ret = putStringAscii(var, curPos);
	if(ret) {
		move(var.length());
	}
	return ret;
}

bool ByteBuffer::putStringAscii(string var, int pos) {
	char* charArr = (char*) var.c_str();
	if(inRange(pos, var.length())) {
		for(size_t i = 0; i < var.length(); i++) {
			(*buf)[pos] = (int16_t) charArr[i];
			pos++;
		}
		return true;
	} else {
		error();
		return false;
	}
}

bool ByteBuffer::putByteArray(vector<int16_t>* arr) {
	if(arr == nullptr) {
		return false;
	}
	if(buf->size() - curPos < arr->size()) {
		resize(curPos + arr->size());
	}
	for(size_t i = 0; i < arr->size(); i++) {
		(*buf)[curPos + i] = (int8_t) (*arr)[i];
	}
	curPos += arr->size();
	return true;
}

bool ByteBuffer::readByteArray(vector<int16_t>* arr) {
	if(arr == nullptr) {
		return false;
	}
	if(buf->size() - curPos < arr->size()) {
		return false;
	}
	for(size_t i = 0; i < arr->size(); i++) {
		(*arr)[i] = (int16_t) ((*buf)[curPos + i] & 0xFF);
	}
	curPos += arr->size();
	return true;
}

bool ByteBuffer::putShortArray(vector<int16_t>* arr) {
	if(arr == nullptr) {
		return false;
	}
	if(buf->size() - curPos < arr->size() * 2) {
		resize(curPos + arr->size() * 2);
	}
	if(byteOrder == BO_BIG_ENDIAN) {
		for(size_t i = 0; i < arr->size(); i++) {
			(*buf)[curPos + 0] = (int16_t) (((*arr)[i] >> 8) & 255);
			(*buf)[curPos + 1] = (int16_t) (((*arr)[i]) & 255);
			curPos += 2;
		}
	} else {
		for(size_t i = 0; i < arr->size(); i++) {
			(*buf)[curPos + 1] = (int16_t) (((*arr)[i] >> 8) & 255);
			(*buf)[curPos + 0] = (int16_t) (((*arr)[i]) & 255);
			curPos += 2;
		}
	}
	return true;
}

string ByteBuffer::toString() {
	char* strBuf = new char(size-1);
	int16_t tmp;
	for(int i = 0; i < (size - 1); i += 2) {
		tmp = (int16_t) (((*buf)[i] << 8) | ((*buf)[i + 1]));
		strBuf[i] = (char) tmp;
	}
	return string(strBuf);
}

string ByteBuffer::toStringAscii() {
	char* strBuf = new char(size-1);
	for(int i = 0; i < size; i++) {
		strBuf[i] = (char) ((*buf)[i]);
	}
	return string(strBuf);
}

bool ByteBuffer::readBoolean() {
	bool ret = readBoolean(curPos);
	move(1);
	return ret;
}

bool ByteBuffer::readBoolean(int pos) {
	return readByte(pos) == 1;
}

int16_t ByteBuffer::readByte() {
	int16_t ret = readByte(curPos);
	move(1);
	return ret;
}

int16_t ByteBuffer::readByte(int pos) {
	if(inRange(pos)) {
		return (*buf)[pos];
	} else {
		error();
		throw "ArrayIndexOutOfBoundsException";
	}
}

int16_t ByteBuffer::readShort() {
	int16_t ret = readShort(curPos);
	move(2);
	return ret;
}

int16_t ByteBuffer::readShort(int pos) {
	if(inRange(pos, 2)) {
		if(this->byteOrder == BO_BIG_ENDIAN) {
			return (int16_t) (((*buf)[pos] << 8) | ((*buf)[pos + 1]));
		} else {
			return (int16_t) (((*buf)[pos + 1] << 8) | ((*buf)[pos]));
		}
	} else {
		error();
		throw "ArrayIndexOutOfBoundsException";
	}
}

int ByteBuffer::readInt() {
	int ret = readInt(curPos);
	move(4);
	return ret;
}

int ByteBuffer::readInt(int pos) {
	int ret = 0;
	if(inRange(pos, 4)) {
		if(this->byteOrder == BO_BIG_ENDIAN) {
			ret |= ((*buf)[pos + 0] << 24);
			ret |= ((*buf)[pos + 1] << 16);
			ret |= ((*buf)[pos + 2] << 8);
			ret |= ((*buf)[pos + 3]);
		} else {
			ret |= ((*buf)[pos + 3] << 24);
			ret |= ((*buf)[pos + 2] << 16);
			ret |= ((*buf)[pos + 1] << 8);
			ret |= ((*buf)[pos + 0]);
		}
		return ret;
	} else {
		error();
		throw "ArrayIndexOutOfBoundsException";
	}
}

char ByteBuffer::readChar() {
	char ret = readChar(curPos);
	move(2);
	return ret;
}

char ByteBuffer::readChar(int pos) {
	if(inRange(pos, 2)) {
		return (char) (readShort(pos));
	} else {
		error();
		throw "ArrayIndexOutOfBoundsException";
	}
}

char ByteBuffer::readCharAscii() {
	char ret = readCharAscii(curPos);
	move(1);
	return ret;
}

char ByteBuffer::readCharAscii(int pos) {
	if(inRange(pos, 1)) {
		return (char) (readByte(pos) & 255);
	} else {
		error();
		throw "ArrayIndexOutOfBoundsException";
	}
}

string ByteBuffer::readString(int length) {
	if(length > 0) {
		string ret = readString(curPos, length);
		move(ret.length() * 2);
		return ret;
	} else {
		return string("");
	}
}

string ByteBuffer::readString(int pos, int length) {
	char* tmp;
	if(inRange(pos, length * 2) && length > 0) {
		tmp = new char[length];
		for(int i = 0; i < length; i++) {
			tmp[i] = readChar(pos + i * 2);
		}
		return string(tmp);
	} else {
		throw "ArrayIndexOutOfBoundsException";
	}
}

string ByteBuffer::readStringWithShortLength() {
	string ret = readStringWithShortLength(curPos);
	move(ret.length() * 2 + 2);
	return ret;
}

string ByteBuffer::readStringWithShortLength(int pos) {
	int16_t len;
	if(inRange(pos, 2)) {
		len = readShort(pos);
		if(len > 0) {
			return readString(pos + 2, len);
		} else {
			return string("");
		}
	} else {
		throw "ArrayIndexOutOfBoundsException";
	}
}

string ByteBuffer::readStringAscii(int length) {
	string ret = readStringAscii(curPos, length);
	move(ret.length());
	return ret;
}

string ByteBuffer::readStringAscii(int pos, int length) {
	char* tmp;
	if(inRange(pos, length) && length > 0) {
		tmp = new char[length];
		for(int i = 0; i < length; i++) {
			tmp[i] = readCharAscii(pos + i);
		}
		return string(tmp);
	} else {
		throw "ArrayIndexOutOfBoundsException";
	}
}

string ByteBuffer::readStringAsciiWithShortLength() {
	string ret = readStringAsciiWithShortLength(curPos);
	move(ret.length() + 2);
	return ret;
}

string ByteBuffer::readStringAsciiWithShortLength(int pos) {
	int16_t len;
	if(inRange(pos, 2)) {
		len = readShort(pos);
		if(len > 0) {
			return readStringAscii(pos + 2, len);
		} else {
			return string("");
		}
	} else {
		throw "ArrayIndexOutOfBoundsException";
	}
}

vector<int16_t>* ByteBuffer::expandShortArray(vector<int16_t>* array, int size) {
	vector<int16_t>* newArr = new vector<int16_t>(array->size() + size);
	for(size_t i=0; i<array->size(); i++)
		(*newArr)[i] = (*array)[i];
	return newArr;
}

void ByteBuffer::crop() {
	if(curPos > 0) {
		if(curPos < buf->size()) {
			buf->resize(curPos);
		}
	} else {
		//System.out.println("Could not crop buffer, as the current position is 0. The buffer may not be empty.");
	}
}

ByteBuffer* ByteBuffer::asciiEncode(ByteBuffer* buf) {

	vector<int16_t>* data = buf->buf;
	vector<int8_t>* enc = new vector<int8_t>(buf->getSize() * 2);

	int encpos = 0;
	int tmp;
	for(size_t i = 0; i < data->size(); i++) {

		tmp = (*data)[i];
		(*enc)[encpos] = (int8_t) ((65 + (tmp)) & 0xF);
		(*enc)[encpos + 1] = (int8_t) ((65 + (tmp >> 4)) & 0xF);
		encpos += 2;

	}
	return new ByteBuffer(enc, ByteBuffer::BO_BIG_ENDIAN);

}
/*
ByteBuffer* ByteBuffer::asciiDecode(ByteBuffer* buf) {
	return nullptr;
}

void ByteBuffer::saveToZipFile(File f, ByteBuffer* buf) {

	try {

		FileOutputStream fOut = new FileOutputStream(f);
		ZipOutputStream zipOut = new ZipOutputStream(fOut);
		zipOut.putNextEntry(new ZipEntry("contents"));
		zipOut.write(buf->getBytes());
		zipOut.closeEntry();
		zipOut.close();
		fOut.close();
	//System.out.println("Buffer was successfully saved to "+f.getPath());

	} catch (Exception e) {

		//System.out.println("Unable to save buffer to file "+f.getPath());
		e.printStackTrace();

	}

}

static ByteBuffer* ByteBuffer::readFromZipFile(File f) {

	try {

		FileInputStream in = new FileInputStream(f);
		ZipInputStream zipIn = new ZipInputStream(in);
		int len, curlen, read;

		ZipFile zip = new ZipFile(f);
		ZipEntry entry = zip.getEntry("contents");
		len = (int) entry.getSize();
		//System.out.println("Len = "+len);

		curlen = 0;
		int8_t* buf = new int8_t[len];
		zipIn.getNextEntry();
		while(curlen < len) {
			read = zipIn.read(buf, curlen, len - curlen);
			if(read >= 0) {
				curlen += read;
			} else {
				// end of file.
				break;
			}
		}
		zipIn.closeEntry();
		zipIn.close();
		in.close();
		zip.close();
		return new ByteBuffer(buf, ByteBuffer.BO_BIG_ENDIAN);

	} catch (Exception e) {
		//System.out.println("Unable to load buffer from file "+f.getPath());
		e.printStackTrace();
	}

	// fail:
	return nullptr;

}
*/

