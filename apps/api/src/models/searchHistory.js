"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.SearchHistory = void 0;
const mongoose_1 = __importDefault(require("mongoose"));
const searchHistorySchema = new mongoose_1.default.Schema({
    userId: { type: mongoose_1.default.Schema.Types.ObjectId, ref: 'User', required: true },
    query: { type: String, required: true }
}, { timestamps: true });
// Limit the number of search history entries per user using an index (not strictly enforcing limit here, but for fast query)
searchHistorySchema.index({ userId: 1, createdAt: -1 });
exports.SearchHistory = mongoose_1.default.model('SearchHistory', searchHistorySchema);
//# sourceMappingURL=searchHistory.js.map