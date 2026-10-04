import mongoose from 'mongoose';

const searchHistorySchema = new mongoose.Schema({
  userId: { type: mongoose.Schema.Types.ObjectId, ref: 'User', required: true },
  query: { type: String, required: true }
}, { timestamps: true });

// Limit the number of search history entries per user using an index (not strictly enforcing limit here, but for fast query)
searchHistorySchema.index({ userId: 1, createdAt: -1 });

export const SearchHistory = mongoose.model('SearchHistory', searchHistorySchema);
