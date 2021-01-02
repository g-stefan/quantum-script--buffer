//
// Quantum Script Extension Buffer
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "quantum-script-context.hpp"
#include "quantum-script-variablenumber.hpp"
#include "quantum-script-variablestring.hpp"
#include "quantum-script-variableobject.hpp"
#include "quantum-script-variablenull.hpp"

#include "quantum-script-extension-buffer.hpp"
#include "quantum-script-extension-buffer-variablebuffer.hpp"


namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Buffer {


				using namespace XYO;

				const char *VariableBuffer::typeBufferKey = "{D28585E9-AA47-4220-820A-AC777702F002}";
				const void *VariableBuffer::typeBuffer;
				const char *VariableBuffer::strTypeBuffer = "Buffer";

				String VariableBuffer::getType() {
					return strTypeBuffer;
				};

				Variable *VariableBuffer::newVariable() {
					return (Variable *) TMemory<VariableBuffer>::newMemory();
				};

				Variable *VariableBuffer::newVariable(size_t size) {
					VariableBuffer *retV;
					retV = TMemory<VariableBuffer>::newMemory();
					retV->buffer.setSize(size);
					return (Variable *) retV;
				};

				Variable *VariableBuffer::newVariable(const uint8_t *data, size_t dataSize) {
					VariableBuffer *retV;
					retV = TMemory<VariableBuffer>::newMemory();
					retV->buffer.set(data, dataSize);
					return (Variable *) retV;
				};

				Variable *VariableBuffer::newVariableFromString(String str_) {
					VariableBuffer *retV;
					retV = TMemory<VariableBuffer>::newMemory();
					retV->buffer.fromString(str_);
					return (Variable *) retV;
				};

				Variable &VariableBuffer::operatorReference(Symbol symbolId) {
					if(symbolId == Context::getSymbolLength()) {
						if(vLength) {
							if(vLength->hasOneReference()) {
								((VariableNumber *)vLength.value())->value = (Number)buffer.length;
							} else {
								vLength=VariableNumber::newVariable((Number)buffer.length);
							};
						} else {
							vLength=VariableNumber::newVariable((Number)buffer.length);
						};
						return *vLength;
					};
					if(symbolId == Context::getSymbolSize()) {
						if(vSize) {
							if(vSize->hasOneReference()) {
								((VariableNumber *)vSize.value())->value = (Number)buffer.size;
							} else {
								vSize=VariableNumber::newVariable((Number)buffer.size);
							};
						} else {
							vSize=VariableNumber::newVariable((Number)buffer.size);
						};
						return *vSize;
					};
					return operatorReferenceX(symbolId, (Extension::Buffer::getContext())->prototypeBuffer->prototype);
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
					if(sscanf((char *)buffer.buffer, QUANTUM_SCRIPT_FORMAT_NUMBER_INPUT, &retV) == 1) {
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
		};
	};
};


