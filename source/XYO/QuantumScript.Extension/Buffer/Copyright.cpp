// Quantum Script Extension Buffer
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Buffer/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Copyright.rh>

namespace XYO::QuantumScript::Extension::Buffer::Copyright {

	static const char *copyright_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_COPYRIGHT;
	static const char *publisher_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_PUBLISHER;
	static const char *company_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_COMPANY;
	static const char *contact_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_CONTACT;
	static const char *fullCopyright_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_FULL_COPYRIGHT;

	const char *copyright() {
		return copyright_;
	};

	const char *publisher() {
		return publisher_;
	};

	const char *company() {
		return company_;
	};

	const char *contact() {
		return contact_;
	};

	const char *fullCopyright() {
		return fullCopyright_;
	};

};
