"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Bookmark = void 0;
const mongoose_1 = __importDefault(require("mongoose"));
const bookmarkSchema = new mongoose_1.default.Schema({
    userId: { type: mongoose_1.default.Schema.Types.ObjectId, ref: 'User', required: true },
    problemId: { type: String, required: true },
    problemTitle: { type: String, required: true },
    difficulty: { type: String, required: true }
}, { timestamps: true });
// Ensure a user can only bookmark a problem once
bookmarkSchema.index({ userId: 1, problemId: 1 }, { unique: true });
exports.Bookmark = mongoose_1.default.model('Bookmark', bookmarkSchema);
//# sourceMappingURL=bookmark.js.map