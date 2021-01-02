//
// Quantum Script Extension Base64
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-base64-license.hpp"
#include "quantum-script-extension-base64.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_NO_VERSION
#include "quantum-script-extension-base64-version.hpp"
#endif

#include "quantum-script-variablestring.hpp"
//
#include "quantum-script-extension-buffer-variablebuffer.hpp"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Base64 {

				using namespace XYO;
				using namespace Quantum::Script;

				static TPointer<Variable> encode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- base64-encode\n");
#endif
					return VariableString::newVariable(XYO::Base64::encode((arguments->index(0))->toString()));
				};

				static TPointer<Variable> decode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- base64-decode\n");
#endif
					String result;
					if(XYO::Base64::decode((arguments->index(0))->toString(), result)) {
						return VariableString::newVariable(result);
					};
					return Context::getValueUndefined();
				};

				static TPointer<Variable> decodeToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- base64-decode-to-buffer\n");
#endif
					String result;
					if(XYO::Base64::decode((arguments->index(0))->toString(), result)) {
						return Extension::Buffer::VariableBuffer::newVariableFromString(result);
					};
					return Context::getValueUndefined();
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("Base64", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "Base64\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "Base64");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::Base64::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX("Script.requireExtension(\"Buffer\");");
					executive->compileStringX("var Base64={};");

					executive->setFunction2("Base64.encode(str)", encode);
					executive->setFunction2("Base64.decode(str)", decode);
					executive->setFunction2("Base64.decodeToBuffer(str)", decodeToBuffer);
				};

			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::Base64::initExecutive(executive, extensionId);
};
#endif

