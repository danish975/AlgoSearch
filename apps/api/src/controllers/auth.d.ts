import { Request, Response } from 'express';
export declare const authController: {
    register(req: Request, res: Response): Promise<Response<any, Record<string, any>> | undefined>;
    login(req: Request, res: Response): Promise<Response<any, Record<string, any>> | undefined>;
    me(req: any, res: Response): Promise<Response<any, Record<string, any>> | undefined>;
};
//# sourceMappingURL=auth.d.ts.map