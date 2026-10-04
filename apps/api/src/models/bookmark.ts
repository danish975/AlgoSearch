import mongoose from 'mongoose';

const bookmarkSchema = new mongoose.Schema({
  userId: { type: mongoose.Schema.Types.ObjectId, ref: 'User', required: true },
  problemId: { type: String, required: true },
  problemTitle: { type: String, required: true },
  difficulty: { type: String, required: true }
}, { timestamps: true });

// Ensure a user can only bookmark a problem once
bookmarkSchema.index({ userId: 1, problemId: 1 }, { unique: true });

export const Bookmark = mongoose.model('Bookmark', bookmarkSchema);
