// Quantum Script Extension Buffer
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Buffer/Library.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Buffer/License.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Context.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>

namespace XYO::QuantumScript::Extension::Buffer {

	BufferContext::BufferContext() {
		symbolFunctionBuffer = 0;
		prototypeBuffer.pointerLink(this);
	};

	BufferContext *getContext() {
		return TSingleton<BufferContext>::getValue();
	};

	static TPointer<Variable> functionBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		Number value = (arguments->index(0))->toNumber();
		if (isnan(value) || isinf(value) || signbit(value)) {
			value = 1024;
		};
		return VariableBuffer::newVariable((size_t)value);
	};

	static void deleteContext() {
		BufferContext *bufferContext = getContext();
		bufferContext->prototypeBuffer.deleteMemory();
		bufferContext->symbolFunctionBuffer = 0;
	};

	static void newContext(Executive *executive, void *extensionId) {
		VariableFunction *defaultPrototypeFunction;

		BufferContext *bufferContext = getContext();
		executive->setExtensionDeleteContext(extensionId, deleteContext);

		bufferContext->symbolFunctionBuffer = Context::getSymbol("Buffer");
		bufferContext->prototypeBuffer.newMemory();

		defaultPrototypeFunction = (VariableFunction *)VariableFunction::newVariable(nullptr, nullptr, nullptr, functionBuffer, nullptr, nullptr);
		(Context::getGlobalObject())->setPropertyBySymbol(bufferContext->symbolFunctionBuffer, defaultPrototypeFunction);
		bufferContext->prototypeBuffer = defaultPrototypeFunction->prototype;
	};

	static TPointer<Variable> getU8(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-get-u8\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number x = (arguments->index(0))->toNumber();

		if (isnan(x) || isinf(x) || signbit(x)) {
			return VariableNumber::newVariable(0);
		};
		if (x >= ((VariableBuffer *)this_)->buffer.length) {
			return VariableNumber::newVariable(0);
		};
		return VariableNumber::newVariable(((VariableBuffer *)this_)->buffer.buffer[(int)x]);
	};

	static TPointer<Variable> setU8(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-set-u8\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number x = (arguments->index(0))->toNumber();
		Number v = (arguments->index(1))->toNumber();

		if (isnan(x) || isinf(x) || signbit(x)) {
			return Context::getValueUndefined();
		};
		if (x >= ((VariableBuffer *)this_)->buffer.size) {
			return Context::getValueUndefined();
		};
		if (isnan(v)) {
			v = 0;
		};
		if (isinf(v)) {
			v = 0xFF;
		};
		if (signbit(v)) {
			v = 0;
		};
		((VariableBuffer *)this_)->buffer.buffer[(int)x] = (uint8_t)v;
		if (x >= ((VariableBuffer *)this_)->buffer.length) {
			((VariableBuffer *)this_)->buffer.length = ((size_t)x + 1);
		};
		return Context::getValueUndefined();
	};

	static TPointer<Variable> set_(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-set\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		bool fill_ = (arguments->index(4))->toBoolean();

		TPointerX<Variable> &inV = arguments->index(1);

		if (TIsType<VariableString>(inV)) {
			String in = inV->toString();
			size_t stx = (arguments->index(0))->toIndex();
			size_t st = (arguments->index(2))->toIndex();
			size_t ln = (arguments->index(3))->toIndex();

			if (stx + ln > ((VariableBuffer *)this_)->buffer.size) {
				if (stx >= ((VariableBuffer *)this_)->buffer.size) {
					return VariableNumber::newVariable(0);
				};
				ln = ((VariableBuffer *)this_)->buffer.size - stx;
			};
			if (ln + st > in.length()) {
				if (st >= in.length()) {
					return VariableNumber::newVariable(0);
				};
				ln = in.length() - st;
			};

			memcpy(&((VariableBuffer *)this_)->buffer.buffer[stx], in.index(st), ln);

			if (((VariableBuffer *)this_)->buffer.length < stx + ln) {
				((VariableBuffer *)this_)->buffer.length = stx + ln;
			};

			if (fill_) {
				if (stx + (arguments->index(3))->toIndex() >= ((VariableBuffer *)this_)->buffer.size) {
					memset(&((VariableBuffer *)this_)->buffer.buffer[ln], 0, ((VariableBuffer *)this_)->buffer.size - ln);
					((VariableBuffer *)this_)->buffer.length = ((VariableBuffer *)this_)->buffer.size;
				} else {
					if (((VariableBuffer *)this_)->buffer.length <= stx + (arguments->index(3))->toIndex()) {
						memset(&((VariableBuffer *)this_)->buffer.buffer[ln], 0, stx + (arguments->index(3))->toIndex() - ln);
						((VariableBuffer *)this_)->buffer.length = stx + (arguments->index(3))->toIndex();
					};
				};
			};
			return VariableNumber::newVariable(ln);
		};

		if (TIsType<VariableBuffer>(inV)) {
			VariableBuffer *in = (VariableBuffer *)inV.value();
			size_t stx = (arguments->index(0))->toIndex();
			size_t st = (arguments->index(2))->toIndex();
			size_t ln = (arguments->index(3))->toIndex();

			if (stx + ln > ((VariableBuffer *)this_)->buffer.size) {
				if (stx >= ((VariableBuffer *)this_)->buffer.size) {
					return VariableNumber::newVariable(0);
				};
				ln = ((VariableBuffer *)this_)->buffer.size - stx;
			};
			if (ln + st > in->buffer.length) {
				if (st >= in->buffer.length) {
					return VariableNumber::newVariable(0);
				};
				ln = in->buffer.length - st;
			};

			memcpy(&((VariableBuffer *)this_)->buffer.buffer[stx], &in->buffer.buffer[st], ln);

			if (((VariableBuffer *)this_)->buffer.length < stx + ln) {
				((VariableBuffer *)this_)->buffer.length = stx + ln;
			};

			if (fill_) {
				if (stx + (arguments->index(3))->toIndex() >= ((VariableBuffer *)this_)->buffer.size) {
					memset(&((VariableBuffer *)this_)->buffer.buffer[((VariableBuffer *)this_)->buffer.length], 0, ((VariableBuffer *)this_)->buffer.size - ((VariableBuffer *)this_)->buffer.length);
					((VariableBuffer *)this_)->buffer.length = ((VariableBuffer *)this_)->buffer.size;
				} else {
					if (((VariableBuffer *)this_)->buffer.length <= stx + (arguments->index(3))->toIndex()) {
						memset(&((VariableBuffer *)this_)->buffer.buffer[((VariableBuffer *)this_)->buffer.length], 0, stx + (arguments->index(3))->toIndex() - ((VariableBuffer *)this_)->buffer.length);
						((VariableBuffer *)this_)->buffer.length = stx + (arguments->index(3))->toIndex();
					};
				};
			};
			return VariableNumber::newVariable(ln);
		};

		return VariableNumber::newVariable(0);
	};

	static TPointer<Variable> fromHex(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-from-hex\n");
#endif
		TPointer<Variable> retV(VariableBuffer::newVariable());
		((VariableBuffer *)retV.value())->buffer.fromHex((arguments->index(0))->toString());
		return retV;
	};

	static TPointer<Variable> toHex(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-to-hex\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		return VariableString::newVariable(((VariableBuffer *)this_)->buffer.toHex());
	};

	static TPointer<Variable> setLength(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-set-length\n");
#endif

		if (TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number x = (arguments->index(0))->toNumber();

		if (isnan(x) || isinf(x) || signbit(x)) {
			return VariableNumber::newVariable(0);
		};
		if (x > ((VariableBuffer *)this_)->buffer.size) {
			return VariableNumber::newVariable(0);
		};

		((VariableBuffer *)this_)->buffer.length = x;
		return VariableNumber::newVariable(((VariableBuffer *)this_)->buffer.length);
	};

	static TPointer<Variable> toString(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-to-string\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		return VariableString::newVariable(this_->toString());
	};

	static TPointer<Variable> fromString(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-from-string\n");
#endif
		return VariableBuffer::newVariableFromString((arguments->index(0))->toString());
	};

	static TPointer<Variable> resize(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
		printf("- buffer-resize\n");
#endif

		if (!TIsType<VariableBuffer>(this_)) {
			throw(Error("invalid parameter"));
		};

		((VariableBuffer *)this_)->resize((arguments->index(0))->toIndex());

		return this_;
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Buffer", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "Buffer\r\n";
		info << License::shortLicense();

		executive->setExtensionName(extensionId, "Buffer");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Buffer::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		newContext(executive, extensionId);

		executive->setFunction2("Buffer.prototype.getU8(pos)", getU8);
		executive->setFunction2("Buffer.prototype.setU8(pos,value)", setU8);
		executive->setFunction2("Buffer.prototype.toHex()", toHex);
		executive->setFunction2("Buffer.prototype.setLength(ln)", setLength);
		executive->setFunction2("Buffer.prototype.toString()", toString);
		executive->setFunction2("Buffer.prototype.set(start,str,strStart,ln,fill)", set_);
		executive->setFunction2("Buffer.prototype.resize(size)", resize);
		//
		executive->setFunction2("Buffer.fromHex(str)", fromHex);
		executive->setFunction2("Buffer.fromString()", fromString);
	};

};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Buffer::initExecutive(executive, extensionId);
};
#endif
