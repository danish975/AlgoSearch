import { Request, Response } from 'express';
export declare const problemsController: {
    getProblem(req: Request, res: Response): Promise<Response<any, Record<string, any>> | undefined>;
    getRelated(req: Request, res: Response): Promise<void>;
    getAlgorithms(req: Request, res: Response): Promise<void>;
    getTopics(req: Request, res: Response): Promise<void>;
};
//# sourceMappingURL=problems.d.ts.map