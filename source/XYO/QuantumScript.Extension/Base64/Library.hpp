// Quantum Script Extension Base64
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_BASE64_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BASE64_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Base64/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Base64 {

	XYO_QUANTUMSCRIPT_EXTENSION_BASE64_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_BASE64_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
