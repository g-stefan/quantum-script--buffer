// Quantum Script Extension Buffer
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <XYO/QuantumScript.Extension/Buffer/Context.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Library.hpp>

namespace XYO::QuantumScript::Extension::Buffer {

	XYO_DYNAMIC_TYPE_IMPLEMENT(VariableBuffer, "{D28585E9-AA47-4220-820A-AC777702F002}");
	const char *VariableBuffer::strTypeBuffer = "Buffer";

	VariableBuffer::VariableBuffer() {
		XYO_DYNAMIC_TYPE_PUSH(VariableBuffer);
	};

	String VariableBuffer::getVariableType() {
		return strTypeBuffer;
	};

	Variable *VariableBuffer::newVariable() {
		return (Variable *)TMemory<VariableBuffer>::newMemory();
	};

	Variable *VariableBuffer::newVariable(size_t size) {
		VariableBuffer *retV;
		retV = TMemory<VariableBuffer>::newMemory();
		retV->buffer.setSize(size);
		return (Variable *)retV;
	};

	Variable *VariableBuffer::newVariable(const uint8_t *data, size_t dataSize) {
		VariableBuffer *retV;
		retV = TMemory<VariableBuffer>::newMemory();
		retV->buffer.set(data, dataSize);
		return (Variable *)retV;
	};

	Variable *VariableBuffer::newVariableFromString(String str_) {
		VariableBuffer *retV;
		retV = TMemory<VariableBuffer>::newMemory();
		retV->buffer.fromString(str_);
		return (Variable *)retV;
	};

	TPointer<Variable> VariableBuffer::getPropertyBySymbol(Symbol symbolId) {
		if (symbolId == Context::getSymbolLength()) {
			return VariableNumber::newVariable((Number)buffer.length);
		};
		if (symbolId == Context::getSymbolSize()) {
			return VariableNumber::newVariable((Number)buffer.size);
		};
		return Variable::getPropertyBySymbol(symbolId);
	};

	bool VariableBuffer::hasPropertyByVariable(Variable *variable) {
		if (TIsType<VariableSymbol>(variable)) {
			if ((static_cast<VariableSymbol *>(variable))->value == Context::getSymbolLength()) {
				return true;
			};
			if ((static_cast<VariableSymbol *>(variable))->value == Context::getSymbolSize()) {
				return true;
			};
		};
		return (Extension::Buffer::getContext())->prototypeBuffer->prototype->hasPropertyByVariable(variable);
	};

	Variable *VariableBuffer::instancePrototype() {
		return (Extension::Buffer::getContext())->prototypeBuffer->prototype;
	};

	void VariableBuffer::initMemory() {
		TMemory<String>::initMemory();
	};

	Variable *VariableBuffer::clone(SymbolList &inSymbolList) {
		VariableBuffer *out = (VariableBuffer *)newVariable(buffer.size);
		out->buffer.copy(buffer);
		return out;
	};

	bool VariableBuffer::toBoolean() {
		return (buffer.length > 0);
	};

	Number VariableBuffer::toNumber() {
		Number retV;
		if (sscanf((char *)buffer.buffer, XYO_QUANTUMSCRIPT_FORMAT_NUMBER_INPUT, &retV) == 1) {
			return retV;
		};
		return NAN;
	};

	String VariableBuffer::toString() {
		String retVX;
		retVX.set((char *)buffer.buffer, buffer.length);
		return retVX;
	};

	bool VariableBuffer::isString() {
		return true;
	};

	void VariableBuffer::resize(size_t size_) {
		buffer.setSize(size_);
	};

};
