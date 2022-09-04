"use strict";

// C library API
const ffi = require("ffi-napi");

// Express App (Routes)
const express = require("express");
const app = express();
app.use(express.json());
const path = require("path");
const fileUpload = require("express-fileupload");
app.use(fileUpload());
app.use(express.static(path.join(__dirname + "/uploads")));

// Minimization
const fs2 = require("node:fs/promises");
const fs = require("fs");
const JavaScriptObfuscator = require("javascript-obfuscator");
const { add } = require("nodemon/lib/rules");
const { create } = require("domain");
const { ffiType } = require("ffi-napi");
const { title } = require("process");

// Important, pass in port as in `npm run dev 1234`, do not change
// portNum = 34747; backup: 30849
const portNum = process.argv[2];

// Setup ffi-napi to connect to C library
let lib = ffi.Library("./parser/libsvgparser", {
	validateSVGWrapper: ["bool", ["string", "string"]],
	createSVGWrapper: ["string", ["string", "string"]],
	getSVGRects: ["string", ["string", "string"]],
	getSVGCircs: ["string", ["string", "string"]],
	getSVGPaths: ["string", ["string", "string"]],
	getSVGGroups: ["string", ["string", "string"]],
	getSVGTitleAndDesc: ["string", ["string", "string"]],
	getRectOtherAttributes: ["string", ["string", "string"]],
	getCircOtherAttributes: ["string", ["string", "string"]],
	getPathOtherAttributes: ["string", ["string", "string"]],
	getGroupOtherAttributes: ["string", ["string", "string"]],
	setAttributeWrapper: [
		"bool",
		["string", "string", "string", "string", "int", "int"],
	],
	addComponentWrapper: ["bool", ["string", "string", "int", "string"]],
	scaleShape: ["bool", ["string", "string", "int", "int"]],
	createNewSVG: ["bool", ["string", "string", "string"]],
});

/* ~~~~~ Given Routes (Leave Alone) ~~~~~ */

// Send HTML at root, do not change
app.get("/", function (req, res) {
	res.sendFile(path.join(__dirname + "/public/index.html"));
});

// Send Style, do not change
app.get("/style.css", function (req, res) {
	//Feel free to change the contents of style.css to prettify your Web app
	res.sendFile(path.join(__dirname + "/public/style.css"));
});

// Send obfuscated JS, do not change
app.get("/index.js", function (req, res) {
	fs.readFile(
		path.join(__dirname + "/public/index.js"),
		"utf8",
		function (err, contents) {
			const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {
				compact: true,
				controlFlowFlattening: true,
			});
			res.contentType("application/javascript");
			res.send(minimizedContents._obfuscatedCode);
		}
	);
});

//Respond to POST requests that upload files to uploads/ directory
app.post("/upload", async (req, res) => {
	if (!req.files) {
		return res.status(400).send("No files were uploaded.");
	}
	let uploadFile = req.files.uploadFile;

	if (!uploadFile.name.toLowerCase().endsWith(".svg")) {
		return res.status(406).send("Not an SVG File, please try again");
	}

	const files = await fs2.readdir(path.join(".", "uploads"));
	if (files.includes(uploadFile.name)) {
		return res.status(406).send("File already exists, please try again");
	}

	// Use the mv() method to place the file somewhere on your server
	uploadFile.mv("uploads/" + uploadFile.name, function (err) {
		if (err) {
			return res.status(500).send(err);
		}
		res.redirect("/load");
	});
});

//Respond to GET requests for files in the uploads/ directory
app.get("/uploads/:name", function (req, res) {
	fs.stat("uploads/" + req.params.name, function (err, stat) {
		if (err == null) {
			res.sendFile(path.join(__dirname + "/uploads/" + req.params.name));
		} else {
			console.log("Error in file downloading route: " + err);
			res.send("");
		}
	});
});

/* ~~~~~ My Routes ~~~~~ */

app.get("/load", async (req, res) => {
	const result = await fs2.readdir(path.join(".", "uploads"));
	const filedata = result.filter((file) => file.endsWith(".svg"));
	const data = [];

	filedata.forEach(async (file, i) => {
		const stats = fs.statSync(`uploads/${file}`);
		const isValidFile = lib.validateSVGWrapper(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);

		if (!isValidFile) {
			data[i] = {
				filename: null,
				stats: null,
				numRects: null,
				numCircles: null,
				numPaths: null,
				numGroups: null,
				valid: false,
			};
		} else {
			const svgString = lib.createSVGWrapper(
				`./uploads/${file}`,
				"./parser/xsd/svg.xsd"
			);

			const { numRect, numCirc, numPaths, numGroups } =
				JSON.parse(svgString);

			data[i] = {
				filename: file,
				stats,
				numRect,
				numCirc,
				numPaths,
				numGroups,
				valid: svgString != null ? true : false,
			};
		}
	});

	const validData = data.filter((svg) => svg.valid != false);
	res.send(validData);
});

app.get("/getUploadedFiles", async (req, res) => {
	const result = await fs2.readdir(path.join(".", "uploads"));
	const filedata = result.filter((file) => file.endsWith(".svg"));
	res.json(filedata);
});

app.get("/getSVGData/:name", async (req, res) => {
	const file = req.params.name;

	const isValidFile = lib.validateSVGWrapper(
		`./uploads/${file}`,
		"./parser/xsd/svg.xsd"
	);

	let data = {};

	if (!isValidFile) {
		console.log("not a valid svg file");
	} else {
		const titleANDdesc = lib.getSVGTitleAndDesc(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);
		const [title, desc] = titleANDdesc.split(":");

		const rectangles = JSON.parse(
			lib.getSVGRects(`./uploads/${file}`, "./parser/xsd/svg.xsd")
		);

		const circles = JSON.parse(
			lib.getSVGCircs(`./uploads/${file}`, "./parser/xsd/svg.xsd")
		);
		const paths = JSON.parse(
			lib.getSVGPaths(`./uploads/${file}`, "./parser/xsd/svg.xsd")
		);
		const groups = JSON.parse(
			lib.getSVGGroups(`./uploads/${file}`, "./parser/xsd/svg.xsd")
		);

		data = {
			valid: true,
			title: title,
			desc: desc,
			rectangles,
			circles,
			paths,
			groups,
		};

		// Get other attributes for all circles in component
		const rectOtherAttr = lib.getRectOtherAttributes(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);
		const rectOtherAttributes = rectOtherAttr.split("|");
		rectOtherAttributes.pop();

		// Get other attributes for all rectangles in component
		const circOtherAttr = lib.getCircOtherAttributes(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);
		const circOtherAttributes = circOtherAttr.split("|");
		circOtherAttributes.pop();

		// Get other attributes for all paths in component
		const pathOtherAttr = lib.getPathOtherAttributes(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);
		const pathOtherAttributes = pathOtherAttr.split("|");
		pathOtherAttributes.pop();

		// Get other attributes for all groups in component
		const groupOtherAttr = lib.getGroupOtherAttributes(
			`./uploads/${file}`,
			"./parser/xsd/svg.xsd"
		);
		const groupOtherAttributes = groupOtherAttr.split("|");
		groupOtherAttributes.pop();

		data.rectangles.forEach((item, i) => {
			item.otherAttributes = rectOtherAttributes[i];
		});
		data.circles.forEach((item, i) => {
			item.otherAttributes = circOtherAttributes[i];
		});
		data.paths.forEach((item, i) => {
			item.otherAttributes = pathOtherAttributes[i];
		});
		data.groups.forEach((item, i) => {
			item.otherAttributes = groupOtherAttributes[i];
		});
	}
	res.send(data);
});

app.post("/setAttribute", async (req, res) => {
	let { file, component, name, value } = req.body;
	let [elementType, index] = component.split(" ");
	--index;

	if (name === "w") name = "width";
	if (name === "h") name = "height";
	if (name === "description") name = "desc";

	if (elementType === "Rectangle") {
		elementType = 2;
	} else if (elementType === "Circle") {
		elementType = 1;
	} else if (elementType === "Path") {
		elementType = 3;
	} else if (elementType === "Group") {
		elementType = 4;
	} else if (elementType === "Svg") {
		elementType = 0;
	}

	const isSuccess = lib.setAttributeWrapper(
		`./uploads/${file}`,
		"./parser/xsd/svg.xsd",
		name,
		value,
		index,
		elementType
	);

	if (!isSuccess) {
		res.status(406).send(
			`Invalid name: ${name} or value ${value} while setting attribute to ${component}`
		);
	} else {
		const data = { file, component, name, value, isSuccess };
		res.status(200).json(data);
	}
});

app.post("/addShape", async (req, res) => {
	let { file, shape } = req.body;

	let elementType = -1;

	if (shape.type === "rectangle") elementType = 2;
	if (shape.type === "circle") elementType = 1;

	if (elementType === -1)
		res.status(404).send("Internal Server Error, please try again");

	if (shape.units === "none") shape.units = "";
	delete shape.type;

	const json = JSON.stringify(shape);

	let isSuccess = lib.addComponentWrapper(
		`./uploads/${file}`,
		"./parser/xsd/svg.xsd",
		elementType,
		json
	);
	console.log(isSuccess);
	if (!isSuccess) {
		res.status(406).send(`Error adding shape to ${file}, try again.`);
	} else {
		const data = { file, shape };
		res.status(200).json(data);
	}
});

app.post("/scaleShape", async (req, res) => {
	const { file, shape, scaleVal } = req.body;
	let elementType = -1;

	if (shape === "rectangle") elementType = 2;
	if (shape === "circle") elementType = 1;

	if (elementType === -1)
		res.status(404).send("Internal Server Error, please try again");

	const isSuccess = lib.scaleShape(
		`./uploads/${file}`,
		"./parser/xsd/svg.xsd",
		elementType,
		scaleVal
	);

	if (!isSuccess) {
		res.status(406).send(`Error trying to scale ${file}`);
	} else {
		const data = { file, shape, scaleVal };
		res.status(200).json(data);
	}
});

app.post("/createSVG", async (req, res) => {
	const { file, json } = req.body;

	const isSuccess = lib.createNewSVG(
		`./uploads/${file}`,
		"./parser/xsd/svg.xsd",
		json
	);

	if (!isSuccess) {
		res.status(406).send("Error creating new svg, try again");
	} else {
		const data = { file };
		res.status(200).json(data);
	}
});

// Port Number to listen on
app.listen(portNum);
console.log("Running app at localhost: " + portNum);
