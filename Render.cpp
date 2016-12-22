#include "Render.h"

namespace Render
{
	void RenderQueue::PushRenderObj(RenderObject* robj, glm::mat4 viewProjMat)
	{
		// TMP : hardcoded GatherRenderOps
		RenderOp rop = robj->rop;
		rop.MVP = viewProjMat*robj->frame.modelMatrix;
		rop.vertexArray = robj->vertexarray;
		rop.program = robj->program;

		renderOps.push_back(rop);
	}

	void RenderQueue::Reset()
	{
		renderOps.clear();
	}

	void RenderQueue::Render()
	{
		for (auto& rop : renderOps)
		{
			Execute(rop, 1);
		}
	}

	void RenderAll(Camera& cam, std::vector<RenderObject*> objects)
	{
		RenderQueue rq;

		for (auto& ro : objects)
		{
			glm::mat4 viewProjMat = cam.projMat * cam.viewMat;

			if (cam.FrustumTest(ro))
			{
				rq.PushRenderObj(ro, viewProjMat);
			}
		}

		rq.Sort();
		rq.Render();

		rq.Reset();
	}
}