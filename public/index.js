"use strict";

let addingCircle = false;
let addingRect = false;

jQuery(document).ready(async () => {
	// Load data
	try {
		const response = await fetch("/load");
		const data = await response.json();
		await updateSVGLists(data);
	} catch (error) {
		console.log(error);
	}

	// Swap between menus, hides and shows content
	const menuItems = document.querySelectorAll(".menu-item");
	for (const item of menuItems) {
		item.addEventListener("click", (e) => {
			e.preventDefault();
			$(item).siblings().removeClass("active-menu-item");
			$(item).addClass("active-menu-item");

			if (item.firstElementChild.textContent == "File Log") {
				$(".svg-viewer-section").addClass("hidden");
				$(".add-shape-modal").addClass("hidden");
				$(".scale-shape-modal").addClass("hidden");
				$(".file-log-section").removeClass("hidden");
			} else if (item.firstElementChild.textContent == "Svg Viewer") {
				$(".file-log-section").addClass("hidden");
				$(".svg-viewer-section").removeClass("hidden");
			}
		});
	}

	// Click confirm button for editing or adding atteribute
	const confirmAttrBtn = document.getElementById("editAttribute");
	confirmAttrBtn.addEventListener("click", addOrUpdateAttributeConfirm);

	// Detecting a change in title or description input to show confirm button
	const svgAttrs = document.querySelectorAll(".svgAttr");
	for (const item of svgAttrs) {
		const prevValue = item.value;
		item.addEventListener("input", () => {
			if (item.value !== prevValue) {
				item.style.width = "90%";
				item.nextElementSibling.style.display = "flex";
				item.nextElementSibling.style.width = "10%";
			} else {
				item.style.width = "100%";
				item.nextElementSibling.style.display = "none";
				item.nextElementSibling.style.width = "0%";
			}
		});
	}

	// Updating an svgs title or description
	const svgAttrConfirmBtns = document.querySelectorAll(".svg-attr-confirm");
	for (const item of svgAttrConfirmBtns) {
		item.addEventListener("click", updateSvgTitleOrDesc);
	}

	// Listener for adding a new shape to svg btn
	const addShapeBtn = document.getElementById("add_shape");
	addShapeBtn.addEventListener("click", () => {
		addingCircle = false;
		addingRect = false;
		const select = document.getElementById("svg_dropdown");
		const curVal = select.options[select.selectedIndex].text;
		document.getElementById(
			"add_shape_filename"
		).firstElementChild.textContent = curVal;
		document.querySelector(".add-shape-modal").classList.remove("hidden");
	});

	// Listener for scaling a shape in svg btn
	const scaleShapeBtn = document.getElementById("scale_shape");
	scaleShapeBtn.addEventListener("click", () => {
		const select = document.getElementById("svg_dropdown");
		const curVal = select.options[select.selectedIndex].text;
		document.getElementById(
			"scale_shape_filename"
		).firstElementChild.textContent = curVal;
		document.querySelector(".scale-shape-modal").classList.remove("hidden");
	});

	const addShapeBtns = document.querySelectorAll(".shape-btn");
	for (const btn of addShapeBtns) {
		btn.addEventListener("click", () => {
			btn.style.backgroundColor = "white";
			btn.style.color = "black";

			if (btn.textContent === "rectangle") {
				addingRect = true;
				addingCircle = false;
				document.querySelector(".circle-attr").classList.add("hidden");
				document.querySelector(".rect-attr").classList.remove("hidden");
			} else {
				addingCircle = true;
				addingRect = false;
				document.querySelector(".rect-attr").classList.add("hidden");
				document
					.querySelector(".circle-attr")
					.classList.remove("hidden");
			}

			for (const tempBtn of addShapeBtns) {
				if (btn != tempBtn) {
					tempBtn.style.backgroundColor = "transparent";
					tempBtn.style.color = "white";
				}
			}
		});
	}

	// Close modal btns
	document
		.getElementById("close_add_shape_modal")
		.addEventListener("click", () => closeModal(".add-shape-modal"));
	document
		.getElementById("close_create_svg_modal")
		.addEventListener("click", () => closeModal(".create-svg-modal"));
	document
		.getElementById("close_scale_shape_modal")
		.addEventListener("click", () => closeModal(".scale-shape-modal"));

	const addShapeToSVGBtn = document.getElementById("add_shape_btn");
	addShapeToSVGBtn.addEventListener("click", addShapeToSVG);
	const scaleShapeInSVGBtn = document.getElementById("scale_shape_btn");
	scaleShapeInSVGBtn.addEventListener("click", scaleShapeInSVG);
	const createNewSVGBtn = document.getElementById("createnew_svg_btn");
	createNewSVGBtn.addEventListener("click", createNewSVG);

	const select = document.getElementById("svg_dropdown");
	select.addEventListener("change", () => {
		const curVal = select.options[select.selectedIndex].text;
		populateSVGViewer(curVal);
	});

	const createSVGBtn = document.getElementById("create_svg_btn");
	createSVGBtn.addEventListener("click", () => {
		document.querySelector(".create-svg-modal").classList.remove("hidden");
	});
});

/**
 * sends request to upload a file to server
 */
const uploadFile = () => {
	const fileInput = document.getElementById("file-input");
	fileInput.click();

	fileInput.onchange = async (e) => {
		const file = e.target.files[0];
		const fd = new FormData();
		fd.append("uploadFile", file);
		try {
			const response = await fetch("/upload", {
				method: "POST",
				body: fd,
			});

			if (!response.ok) {
				const data = await response.text();
				alert(data);
			} else {
				location.reload();
			}
		} catch (error) {
			alert(error);
		}
	};
};

/**
 * adds table cell to table row
 * @param {*} row
 * @param {*} cellData
 * @param {*} className
 */
const addCellToRow = (row, cellData, className) => {
	const tableData = row.insertCell();
	tableData.classList.add(className);
	const content = document.createTextNode(cellData);
	tableData.appendChild(content);
};

/**
 * Turns input background to a red/pink colour for invalidness
 * @param {*} element
 * @param {*} text
 */
const showInvalidColour = (element, text) => {
	const prevColor = element.style.backgroundColor;
	element.style.backgroundColor = "#FFCCCC";
	const prevPh = element.placeholder !== undefined ? element.placeholder : "";
	if (text !== undefined && element.nodeName == "INPUT") {
		element.placeholder = text;
	}
	setTimeout(
		() => {
			element.style.backgroundColor = prevColor;
			if (text !== undefined && element.nodeName == "INPUT") {
				element.placeholder = prevPh;
			}
		},
		text !== undefined ? 1300 : 500
	);
};

/**
 * Turns input background to a greenish colour for validness
 * @param {*} element
 */
const showValidColour = (element) => {
	const prevColor = element.style.backgroundColor;
	element.style.backgroundColor = "lightgreen";
	setTimeout(() => {
		element.style.backgroundColor = prevColor;
	}, 1500);
};
/**
 * will update the info in the filelog and svg viewer
 * @param {Object} data
 */
const updateSVGLists = (data) => {
	populateFileLog(data);
	if (Object.keys(data).length > 0) {
		for (const svg of data) {
			const dropdown = document.getElementById("svg_dropdown");
			const option = document.createElement("option");
			if (svg.valid) {
				option.textContent = svg.filename;
				option.value = svg.filename;
				dropdown.appendChild(option);
			}
		}
		populateSVGViewer(data[0].filename);
	}
};

/**
 * populates the svg viewer section with all details for the given file
 * @param {string} filename
 */
const populateSVGViewer = async (filename) => {
	//make get req to get all data required for an svg

	let data = {};
	try {
		const response = await fetch(`/getSVGData/${filename}`);
		data = await response.json();
	} catch (error) {
		console.log(error);
	}

	document.getElementById("svg_view_img").src =
		`./uploads/${filename}?x=` + performance.now();
	document.getElementById("title_attr").value = data.title;
	document.getElementById("desc_attr").value = data.desc;

	const componentsWrap = document.querySelector(".components-wrap");
	while (componentsWrap.firstChild) {
		componentsWrap.firstChild.remove();
	}

	addComponentToSvgView(data.rectangles, "Rectangle", "Upper left corner");
	addComponentToSvgView(data.circles, "Circle", "Center");
	addComponentToSvgView(data.paths, "Path", "Path data");
	addComponentToSvgView(data.groups, "Group", "details");

	// when click an attribute it will fill the name input with it
	const attributesList = document.querySelectorAll(".attribute");
	for (const attr of attributesList) {
		attr.addEventListener("click", () => {
			const name = attr.textContent.split("=")[0].trim();
			document.getElementById("attrName").value = name;
			document.querySelector(".attributeName").value =
				attr.parentElement.parentElement.firstElementChild.textContent;
		});
	}
};

/**
 * populates the file log with all required info
 * @param {Object} data
 * @returns
 */
const populateFileLog = (data) => {
	const tbody = document.getElementById("tbody");

	if (Object.keys(data).length < 1) {
		const row = tbody.insertRow();
		addCellToRow(row, "No Files", null);
		return;
	}
	for (const svg of data) {
		if (!svg.valid) return;

		const row = tbody.insertRow();
		// Add svg img to row
		const td = row.insertCell();

		const a = document.createElement("a");
		a.href = `/uploads/${svg.filename}`;
		a.download = "quad01"; //todo: fix this status strin/value

		const svgImg = document.createElement("img");
		svgImg.classList.add("image");
		svgImg.src = `/uploads/${svg.filename}`;
		a.appendChild(svgImg);
		td.appendChild(a);

		const tdFilename = row.insertCell();
		const filename = document.createElement("a");
		filename.classList.add("filename");
		filename.href = `/uploads/${svg.filename}`;
		filename.download = svg.filename.split(".")[0];
		filename.textContent = svg.filename;
		tdFilename.appendChild(filename);

		addCellToRow(
			row,
			`${Math.round(svg.stats.size / 1000)}kb`,
			"file-size"
		);
		addCellToRow(row, svg.numRect, "num-rects");
		addCellToRow(row, svg.numCirc, "num-circles");
		addCellToRow(row, svg.numPaths, "num-paths");
		addCellToRow(row, svg.numGroups, "num-groups");
	}
};

/**
 * sends request to scale a shape
 */
const scaleShapeInSVG = async () => {
	const addShapeBtns = document.querySelectorAll(".shape-btn");
	if (!addingRect && !addingCircle) {
		showInvalidColour(addShapeBtns[2]);
		showInvalidColour(addShapeBtns[3]);
		return;
	}

	const select = document.getElementById("svg_dropdown");
	const svgFileToAddTo = select.options[select.selectedIndex].text;
	const scaleVal = document.getElementById("scale_shape_input").value;
	let data = {};

	if (addingRect) {
		data = {
			file: svgFileToAddTo,
			shape: "rectangle",
			scaleVal,
		};
	} else if (addingCircle) {
		data = {
			file: svgFileToAddTo,
			shape: "circle",
			scaleVal,
		};
	} else {
		console.error("Internal error, try again");
		return;
	}

	try {
		const response = await fetch("/scaleShape", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify(data),
		});

		if (!response.ok) {
			const data = await response.text();
			alert(data);
			location.reload();
		} else {
			const data = await response.json();

			// Update img src in svg viewer and file log
			const table = document.getElementById("tbody").children;
			for (const item of table) {
				const file =
					item.firstElementChild.nextElementSibling.textContent;
				if (file === data.file) {
					item.firstElementChild.firstElementChild.firstElementChild.src =
						`./uploads/${data.file}?x=` + performance.now();
					break;
				}
			}

			const modal = document.querySelector(".scale-shape-modal");
			clearModalValues(modal);
			modal.classList.add("hidden");

			populateSVGViewer(data.file);
		}
	} catch (error) {
		console.error(error);
	}
};

/**
 * sends request ot add shape to an svg
 */
const addShapeToSVG = async () => {
	const addShapeBtns = document.querySelectorAll(".shape-btn");
	if (!addingRect && !addingCircle) {
		showInvalidColour(addShapeBtns[0]);
		showInvalidColour(addShapeBtns[1]);
		return;
	}

	const rectXAttr = document.getElementById("rect-attr-x");
	const rectYAttr = document.getElementById("rect-attr-y");
	const rectWAttr = document.getElementById("rect-attr-w");
	const rectHAttr = document.getElementById("rect-attr-h");
	const rectSelect = document.getElementById("rect-attr-units");
	const rectUnitsVal = rectSelect.options[rectSelect.selectedIndex].text;
	const circCXAttr = document.getElementById("circ-attr-cx");
	const circCYAttr = document.getElementById("circ-attr-cy");
	const circRAttr = document.getElementById("circ-attr-r");
	const circSelect = document.getElementById("circ-attr-units");
	const circUnitsVal = circSelect.options[circSelect.selectedIndex].text;

	if (addingRect) {
		if (
			rectXAttr.value == null ||
			rectXAttr.value == "" ||
			isNaN(rectXAttr.value)
		) {
			showInvalidColour(rectXAttr, "Invalid x value");
			return;
		}
		if (
			rectYAttr.value == null ||
			rectYAttr.value == "" ||
			isNaN(rectYAttr.value)
		) {
			showInvalidColour(rectYAttr, "Invalid y value");
			return;
		}
		if (
			rectWAttr.value == null ||
			rectWAttr.value == "" ||
			rectWAttr.value < 0 ||
			isNaN(rectWAttr.value)
		) {
			showInvalidColour(rectWAttr, "Invalid width");
			return;
		}
		if (
			rectHAttr.value == null ||
			rectHAttr.value == "" ||
			rectHAttr.value < 0 ||
			isNaN(rectHAttr.value)
		) {
			showInvalidColour(rectHAttr);
			return;
		}
		if (rectUnitsVal == null || rectUnitsVal == "units") {
			showInvalidColour(rectSelect, "Invalid height");
			return;
		}
	}

	if (addingCircle) {
		if (
			circCXAttr.value == null ||
			circCXAttr.value == "" ||
			isNaN(circCXAttr.value)
		) {
			showInvalidColour(circCXAttr, "Invalid cx value");
			return;
		}
		if (
			circCYAttr.value == null ||
			circCYAttr.value == "" ||
			isNaN(circCYAttr.value)
		) {
			showInvalidColour(circCYAttr, "Invalid cy value");
			return;
		}
		if (
			circRAttr.value == null ||
			circRAttr.value == "" ||
			isNaN(circRAttr.value) ||
			circRAttr.value < 0
		) {
			showInvalidColour(circRAttr);
			return;
		}
		if (circUnitsVal == null || circUnitsVal == "units") {
			showInvalidColour(circSelect, "Invalid radius");
			return;
		}
	}

	// Everything is valid now, we can send post request to add shape
	const select = document.getElementById("svg_dropdown");
	const svgFileToAddTo = select.options[select.selectedIndex].text;

	let data = { file: svgFileToAddTo };
	let shape = {};

	if (addingRect) {
		shape = {
			type: "rectangle",
			x: rectXAttr.value,
			y: rectYAttr.value,
			w: rectWAttr.value,
			h: rectHAttr.value,
			units: rectUnitsVal,
		};
	} else if (addingCircle) {
		shape = {
			type: "circle",
			cx: circCXAttr.value,
			cy: circCYAttr.value,
			r: circRAttr.value,
			units: circUnitsVal,
		};
	} else {
		console.error("Internal error, try again");
		return;
	}

	data.shape = shape;

	try {
		const response = await fetch("/addShape", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify(data),
		});

		if (!response.ok) {
			const data = await response.text();
			alert(data);
			location.reload();
		} else {
			// Update img src in svg viewer and file log
			const table = document.getElementById("tbody").children;
			for (const item of table) {
				const file =
					item.firstElementChild.nextElementSibling.textContent;
				if (file === data.file) {
					item.firstElementChild.firstElementChild.firstElementChild.src =
						`./uploads/${data.file}?x=` + performance.now();
					break;
				}
			}

			const modal = document.querySelector(".add-shape-modal");
			clearModalValues(modal);
			modal.classList.add("hidden");

			populateSVGViewer(data.file);
		}
	} catch (error) {
		console.error(error);
	}
};

/**
 * sends request to create a new svg if its valid
 */
const createNewSVG = async () => {
	const filenameInput = document.getElementById("create_svg_filename");

	if (
		filenameInput == null ||
		filenameInput.value === undefined ||
		filenameInput.value == "" ||
		!filenameInput.value.toLowerCase().endsWith(".svg") ||
		filenameInput.value.toLowerCase() === ".svg"
	) {
		showInvalidColour(filenameInput, "Please enter a value");
		return;
	}

	try {
		const response = await fetch("/getUploadedFiles");
		const data = await response.json();
		if (data.includes(filenameInput.value)) {
			alert("file already exists");
			return;
		}
	} catch (error) {
		console.error(error);
	}

	const errorBox = document.getElementById("error_box"); // put msg in here for what went wrong if fails
	const title = document.getElementById("new_svg_title").value;
	const descr = document.getElementById("new_svg_desc").value;

	const data = {
		file: filenameInput.value,
		json: JSON.stringify({
			title,
			descr,
		}),
	};

	try {
		const response = await fetch("/createSVG", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify(data),
		});

		if (!response.ok) {
			const data = await response.text();
			alert(data);
			location.reload();
		} else {
			const data = await response.json();
			location.reload();
		}
	} catch (error) {
		console.error(error);
	}
};

/**
 * Adds all components of a certain shape to the svg viewer for the svg
 * @param {*} list
 * @param {*} shapeText
 * @param {*} shapeWriting
 */
const addComponentToSvgView = (list, shapeText, shapeWriting) => {
	const componentsWrap = document.querySelector(".components-wrap");

	// Loop all components
	for (const [i, shape] of list.entries()) {
		const component = document.createElement("div");
		component.classList.add("component");

		const name = document.createElement("p");
		name.textContent = `${shapeText} ${i + 1}`;
		const label = document.createElement("label");
		label.textContent = shapeWriting;

		const attributes = document.createElement("div");
		attributes.classList.add("attributes");

		// Create all primary attributes for the element
		for (const [key, value] of Object.entries(shape)) {
			if (
				key != "numAttr" &&
				key != "units" &&
				key != "otherAttributes"
			) {
				const attribute = document.createElement("div");
				attribute.classList.add("attribute");
				attribute.textContent = `${key} = ${value}${
					shape.units === undefined ? "" : shape.units
				}`;
				attributes.appendChild(attribute);
			}
		}

		const br = document.createElement("br");

		const label2 = document.createElement("label");
		label2.textContent = `${shape.numAttr} other attributes`;

		const otherAttributes = document.createElement("div");
		otherAttributes.classList.add("attributes");

		shape.otherAttributes = JSON.parse(shape.otherAttributes);

		// create all other attributes for the element
		for (const attr of shape.otherAttributes) {
			const attribute = document.createElement("div");
			attribute.textContent = `${attr.name} = ${attr.value}`;
			attribute.classList.add("attribute");
			otherAttributes.appendChild(attribute);
		}

		component.appendChild(name);
		component.appendChild(label);
		component.appendChild(attributes);
		component.appendChild(br);
		component.appendChild(label2);
		component.appendChild(otherAttributes);
		componentsWrap.appendChild(component);
		componentsWrap.appendChild(document.createElement("hr"));
	}
};

/**
 * function that sends request to update or add attribute to a component
 */
const addOrUpdateAttributeConfirm = async () => {
	// Do any validation neccesary
	const name = document.getElementById("attrName");
	const value = document.getElementById("attrValue");
	const componentToChange = document.querySelector(".attributeName");

	if (componentToChange.value.length == 0) {
		showInvalidColour(componentToChange, "component name");
		return;
	}

	if (name.value.length === 0) {
		showInvalidColour(name);
		return;
	}

	if (value.value.length === 0) {
		showInvalidColour(value);
		return;
	}

	const requiredAttr =
		name.value.toLowerCase() === "x" ||
		name.value.toLowerCase() === "y" ||
		name.value.toLowerCase() === "w" ||
		name.value.toLowerCase() === "h" ||
		name.value.toLowerCase() === "cx" ||
		name.value.toLowerCase() === "cy" ||
		name.value.toLowerCase() === "r";

	const attrWithRestriction =
		name.value.toLowerCase() === "w" ||
		name.value.toLowerCase() === "h" ||
		name.value.toLowerCase() === "r";

	if (requiredAttr) {
		if (attrWithRestriction && !isNaN(value.value) && value.value < 0) {
			showInvalidColour(value);
			const prevVal = value.value;
			value.value = "Must be positive";
			value.style.color = "black";
			setTimeout(() => {
				value.value = prevVal;
				value.style.color = "white";
			}, 500);
			return;
		} else if (isNaN(value.value)) {
			showInvalidColour(value);
			const prevVal = value.value;
			value.value = "Must be a number";
			value.style.color = "black";
			setTimeout(() => {
				value.value = prevVal;
				value.style.color = "white";
			}, 500);
			return;
		}
	}

	const componentToEdit = document.querySelector(".attributeName").value;
	const select = document.getElementById("svg_dropdown");
	const svgFileToAddTo = select.options[select.selectedIndex].text;

	const data = {
		file: svgFileToAddTo,
		component: componentToEdit,
		name: name.value,
		value: value.value,
	};

	//send request here to update/add
	try {
		const response = await fetch("/setAttribute", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify(data),
		});

		// clear inputs
		document.getElementById("attrName").value = "";
		document.getElementById("attrValue").value = "";

		if (!response.ok) {
			const msg = await response.text();

			const errorBox = document.createElement("div");
			errorBox.classList.add("error-box");
			document.body.appendChild(errorBox);

			const errorMsg = document.createElement("p");
			errorMsg.textContent = msg;
			errorBox.appendChild(errorMsg);

			setTimeout(() => {
				document.body.removeChild(errorBox);
			}, 6000);

			document.querySelector(".attributeName").value = "";
		} else {
			const data = await response.json();

			const components =
				document.querySelector(".components-wrap").children;
			for (const component of components) {
				if (component.nodeName == "HR") continue;
				if (
					component.firstElementChild.textContent === data.component
				) {
					let found = false;
					const reqAttrs =
						component.firstElementChild.nextElementSibling
							.nextElementSibling.children;

					for (let attr of reqAttrs) {
						let unit = attr.textContent
							.split("=")[1]
							.replace(/[0-9]/g, "")
							.substring(1);
						if (
							attr.textContent.split("=")[0].trim() ==
							data.name.charAt(0)
						) {
							attr.textContent = `${data.name.charAt(0)} = ${
								data.value
							}${unit}`;
							found = true;
							break;
						}
					}
					if (found) break;
					const otherAttrs = component.lastElementChild.children;
					for (let attr of otherAttrs) {
						if (
							attr.textContent.split("=")[0].trim() === data.name
						) {
							attr.textContent = `${data.name} = ${data.value}`;
							found = true;
							break;
						}
						if (found) break;
					}

					if (found) break;

					// If reached here then its a new attribute they are adding
					const otherAttrParent = component.lastElementChild;
					const attr = document.createElement("div");
					attr.classList.add("attribute");
					attr.textContent = `${data.name} = ${data.value}`;
					otherAttrParent.appendChild(attr);
				}
			}

			// Update img src in svg viewer and file log
			document.getElementById("svg_view_img").src =
				`./uploads/${data.file}?x=` + performance.now();

			const table = document.getElementById("tbody").children;
			for (const item of table) {
				const file =
					item.firstElementChild.nextElementSibling.textContent;
				if (file === data.file) {
					item.firstElementChild.firstElementChild.firstElementChild.src =
						`./uploads/${data.file}?x=` + performance.now();
					break;
				}
			}

			showValidColour(document.getElementById("attrName"));
			showValidColour(document.getElementById("attrValue"));
		}
	} catch (error) {
		console.error(error);
	}
};

/**
 * sends request to update title or desc of an svg
 */
const updateSvgTitleOrDesc = async () => {
	const name =
		item.parentElement.previousElementSibling.textContent.toLowerCase();
	let value = item.parentElement.firstElementChild.value;

	//validate length of the string
	if (value.length > 256) {
		showInvalidColour(item.parentElement.firstElementChild);
		return;
	}

	const select = document.getElementById("svg_dropdown");
	const svgFileToAddTo = select.options[select.selectedIndex].text;

	const data = {
		file: svgFileToAddTo,
		component: "Svg 0",
		name: name,
		value,
	};

	//send request to update here
	try {
		const response = await fetch("/setAttribute", {
			method: "POST",
			headers: {
				"Content-Type": "application/json",
			},
			body: JSON.stringify(data),
		});

		if (!response.ok) {
			const data = await response.text();
			console.log("not valid");
			// const data = await response.text();
		} else {
			const data = await response.json();
			value = data.value;
			showValidColour(item.parentElement.firstElementChild);
		}
	} catch (error) {
		console.log(error);
	}
};

/**
 * hides a modal and clears its values
 * @param {*} modalClass
 */
const closeModal = (modalClass) => {
	const modal = document.querySelector(modalClass);
	modal.classList.add("hidden");
	clearModalValues(modal);
};

/**
 * clears the values in a modal
 * @param {*} modal
 */
const clearModalValues = (modal) => {
	const inputs = modal.getElementsByTagName("input");

	for (const input of inputs) {
		input.value = "";
	}

	const addShapeBtns = document.querySelectorAll(".shape-btn");

	for (const btn of addShapeBtns) {
		addingCircle = false;
		addingRect = false;
		btn.style.backgroundColor = "transparent";
		btn.style.borderColor = "white";
		btn.style.color = "white";
	}

	const rectSelect = document.getElementById("rect-attr-units");
	const circSelect = document.getElementById("circ-attr-units");
	rectSelect.selectedIndex = 0;
	circSelect.selectedIndex = 0;
};
