"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.errorHandler = void 0;
const express_1 = require("express");
const logger_1 = require("../utils/logger");
const errorHandler = (err, req, res, next) => {
    logger_1.logger.error('Unhandled error', err);
    res.status(500).json({ error: 'Internal Server Error' });
};
exports.errorHandler = errorHandler;
//# sourceMappingURL=errorHandler.js.map