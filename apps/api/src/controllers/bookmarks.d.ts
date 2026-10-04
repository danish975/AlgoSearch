import { Response } from 'express';
export declare const bookmarksController: {
    getBookmarks(req: any, res: Response): Promise<void>;
    addBookmark(req: any, res: Response): Promise<Response<any, Record<string, any>> | undefined>;
    removeBookmark(req: any, res: Response): Promise<void>;
};
//# sourceMappingURL=bookmarks.d.ts.map