// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Base64");

var checkEncoding = [
		["", ""],
		["f", "Zg=="],
		["fo", "Zm8="],
		["foo", "Zm9v"],
		["foob", "Zm9vYg=="],
		["fooba", "Zm9vYmE="],
		["foobar", "Zm9vYmFy"]
	];

var allOk = true;
for(var k of checkEncoding) {
	if(Base64.encode(k[0]) != k[1]) {
		allOk = false;
		Console.writeLn("Error-Encode: " + k[0] + " : " + k[1] + " : " + Base64.encode(k[0]));
	};
};

for(var k of checkEncoding) {
	if(Base64.decode(k[1]) != k[0]) {
		allOk = false;
		Console.writeLn("Error-Decode: " + k[1] + " : " + k[0] + " : " + Base64.decode(k[1]));
	};
};

if(allOk) {
	Console.writeLn("-> test 0001 ok");
};
