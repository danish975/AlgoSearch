"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.searchRouter = void 0;
const express_1 = require("express");
const search_1 = require("../../controllers/search");
const auth_1 = require("../../middleware/auth");
exports.searchRouter = (0, express_1.Router)();
// Optional auth for history tracking
exports.searchRouter.get('/', (req, res, next) => {
    if (req.headers.authorization) {
        (0, auth_1.authenticate)(req, res, () => search_1.searchController.search(req, res));
    }
    else {
        search_1.searchController.search(req, res);
    }
});
exports.searchRouter.get('/suggestions', search_1.searchController.autocomplete);
//# sourceMappingURL=search.js.map